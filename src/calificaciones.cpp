#include "modulos.h"
#include "recomendador.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

// =====================================================================
// Integracion: carga de calificaciones de la comunidad y cambio de usuario.
//
// Archivo data/calificaciones.txt, una fila por calificacion:
//   idUsuario,idPelicula,estrellas
//
// Cada fila valida se reparte a los modulos duenios de cada dato:
//   - Int.4 Grafo   : grafoRegistrarCalificacion (red social, Jaccard)
//   - Int.1 Hash    : registrarVoto (ratingPromedio y numVotos de la pelicula)
//   - Int.2 Perfil  : se recarga el historial del usuario activo
// El grafo guarda las calificaciones de TODOS los usuarios, por eso al cambiar
// de usuario activo su historial se recupera desde ahi (grafoCalificacionesDe).
// =====================================================================

static string sinRetornoCarro(const string& s) {
    if (!s.empty() && s[s.size() - 1] == '\r')
        return s.substr(0, s.size() - 1);
    return s;
}

// Convierte texto a entero estricto (solo digitos). false si no es un numero.
static bool aEntero(const string& texto, int& valor) {
    size_t i = 0;
    while (i < texto.size() && (texto[i] == ' ' || texto[i] == '\t')) i++;
    size_t inicio = i;
    while (i < texto.size() && texto[i] >= '0' && texto[i] <= '9') i++;
    size_t fin = i;
    while (i < texto.size() && (texto[i] == ' ' || texto[i] == '\t')) i++;
    if (fin == inicio || i != texto.size() || fin - inicio > 9)
        return false;
    valor = atoi(texto.substr(inicio, fin - inicio).c_str());
    return true;
}

// Recarga el perfil (Int.2) del usuario indicado con lo que hay en el grafo.
// Devuelve cuantas calificaciones cargo.
static int recargarPerfil(int idUsuario) {
    perfilCambiarUsuario(idUsuario);

    int capacidad = 64;
    Calificacion* buffer = nullptr;
    int n = 0;
    while (true) {
        buffer = new Calificacion[capacidad];
        n = grafoCalificacionesDe(idUsuario, buffer, capacidad);
        if (n < capacidad) break;
        delete[] buffer;
        capacidad *= 2;
    }
    // El grafo las da en orden ascendente de id; el historial inserta por la
    // cabeza, asi que se recorren al reves para que quede ordenado por id.
    for (int i = n - 1; i >= 0; i--)
        perfilCargarCalificacion(buffer[i].idPelicula, buffer[i].estrellas);
    delete[] buffer;
    return n;
}

int cargarCalificaciones(const string& ruta) {
    ifstream archivo(ruta.c_str());
    if (!archivo.is_open())
        return -1;

    int validas = 0;
    string linea;
    while (getline(archivo, linea)) {
        linea = sinRetornoCarro(linea);
        if (linea.empty() || linea[0] == '#') continue;

        stringstream ss(linea);
        string sUsuario, sPelicula, sEstrellas;
        getline(ss, sUsuario, ',');
        getline(ss, sPelicula, ',');
        getline(ss, sEstrellas, ',');

        int idUsuario, idPelicula, estrellas;
        if (!aEntero(sUsuario, idUsuario) || !aEntero(sPelicula, idPelicula) ||
            !aEntero(sEstrellas, estrellas))
            continue;                                   // encabezado o fila mal formada
        if (idUsuario <= 0 || estrellas < 1 || estrellas > 5)
            continue;
        if (obtenerPelicula(idPelicula) == nullptr)
            continue;                                   // pelicula fuera del catalogo

        // Si la fila repite (usuario, pelicula), se reemplaza el voto anterior
        int anteriores = grafoEstrellasDe(idUsuario, idPelicula);
        grafoRegistrarCalificacion(idUsuario, idPelicula, estrellas);
        registrarVoto(idPelicula, estrellas, anteriores);
        validas++;
    }

    recargarPerfil(usuarioActivo());
    return validas;
}

string cambiarUsuario(int idUsuario) {
    if (idUsuario <= 0)
        return "ERROR: el ID de usuario debe ser mayor que 0.";
    if (idUsuario == usuarioActivo())
        return "Ya eres el usuario " + to_string(idUsuario) + ".";

    int cargadas = recargarPerfil(idUsuario);
    recomendadorReiniciar();     // la ultima lista de 'explicar' era del usuario anterior

    ostringstream o;
    o << "Usuario activo: " << idUsuario << " (" << cargadas << " calificacion"
      << (cargadas == 1 ? "" : "es") << " cargada" << (cargadas == 1 ? "" : "s") << ")";
    if (cargadas == 0)
        o << ". Usuario nuevo: califica peliculas para personalizar tus recomendaciones.";
    return o.str();
}
