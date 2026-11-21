#include "modelo.h"
#include "modulos.h"
#include "afinidad.h"
#include "grafo.h"
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace std;

// ================= CONFIGURACION Y ESTADO DEL MODULO =================

// Cantidad maxima de vecinos mas afines usados en el calculo colaborativo.
static const int K_VECINOS_COLABORATIVOS = 10;

static bool modoDevLocal = false;
static GrafoAfinidad redSocial;

void afinidadModoDev(bool activo) {
    modoDevLocal = activo;
}

// ================= AUXILIARES =================

// Formatea el titulo de una pelicula usando el catalogo real del Int.1 (Hash O(1)).
// Si obtenerPelicula devuelve nullptr (catalogo no cargado o stub), usa el ID.
static string describirPelicula(int idPelicula, bool incluirAnio) {
    const Pelicula* p = obtenerPelicula(idPelicula);
    ostringstream o;
    if (p != nullptr) {
        o << "[" << p->id << "] " << p->titulo;
        if (incluirAnio)
            o << " (" << p->anio << ") - " << p->genero;
        else
            o << " (" << p->genero << ")";
    } else {
        o << "[" << idPelicula << "] Pelicula #" << idPelicula;
    }
    return o.str();
}

// ================= CONTRATO v2 (modulos.h) =================

// Registra, actualiza o elimina una calificacion en el grafo social.
// Convencion obligatoria: estrellas == 0 elimina la pelicula (usado por deshacer).
// No reconstruye aristas de inmediato: marca el grafo como sucio para hacerlo
// una sola vez antes de la siguiente consulta. Complejidad: O(V + |H|).
void grafoRegistrarCalificacion(int idUsuario, int idPelicula, int estrellas) {
    if (idUsuario <= 0 || idPelicula <= 0)
        return;

    if (estrellas <= 0) {
        redSocial.eliminarPelicula(idUsuario, idPelicula);
        return;
    }

    if (estrellas > 5)
        return;

    if (redSocial.buscarVertice(idUsuario) == nullptr) {
        redSocial.insertarUsuario(idUsuario, "Usuario " + to_string(idUsuario));
    }

    redSocial.agregarPelicula(idUsuario, idPelicula, estrellas);
}

// Estrellas que idUsuario le dio a idPelicula segun el grafo (0 si no la califico).
// Complejidad: O(V + |H|)
int grafoEstrellasDe(int idUsuario, int idPelicula) {
    return redSocial.obtenerEstrellas(idUsuario, idPelicula);
}

// Copia en destino (hasta max) las calificaciones guardadas en el grafo para
// idUsuario, en orden ascendente de idPelicula. Devuelve cuantas copio.
// El grafo es el "almacen" de calificaciones de toda la comunidad, asi al cambiar
// de usuario activo se recupera su historial sin duplicar datos. O(V + |H|)
int grafoCalificacionesDe(int idUsuario, Calificacion* destino, int max) {
    if (destino == nullptr || max <= 0)
        return 0;
    VerticeUsuario* v = redSocial.buscarVertice(idUsuario);
    if (v == nullptr)
        return 0;
    int copiadas = 0;
    NodoPelicula* p = v->historial;
    while (p != nullptr && copiadas < max) {
        destino[copiadas].idUsuario = idUsuario;
        destino[copiadas].idPelicula = p->idPelicula;
        destino[copiadas].estrellas = p->estrellas;
        copiadas++;
        p = p->sig;
    }
    return copiadas;
}

// Cantidad de usuarios (vertices) registrados en la red social. O(1)
int grafoTotalUsuarios() {
    return redSocial.totalVertices();
}

// Devuelve un puntaje colaborativo en [0.0, 1.0] para idPelicula respecto
// al usuario activo, ponderando los k vecinos mas afines en el grafo.
// Usa el cache interno de GrafoAfinidad para responder rapido las ~749 llamadas
// de recomendar(). Complejidad amortizada: O(k * |H_v|).
double puntajeColaborativo(int idPelicula) {
    int activo = usuarioActivo();
    if (activo <= 0 || idPelicula <= 0)
        return 0.0;

    return redSocial.calcularPuntajeColaborativo(activo, idPelicula, K_VECINOS_COLABORATIVOS);
}

// Genera el reporte de afinidad social (Jaccard sobre peliculas >= 4 estrellas)
// entre el usuario activo y el usuario indicado, mas recomendaciones colaborativas.
string afinidad(int usuario) {
    if (usuario <= 0)
        return "ERROR: el ID de usuario a comparar debe ser mayor que 0.";

    int activo = usuarioActivo();
    if (activo <= 0)
        return "ERROR: no hay un usuario activo valido.";

    if (usuario == activo)
        return "ERROR: no puedes calcular afinidad contigo mismo.";

    auto inicio = chrono::high_resolution_clock::now();

    bool reconstruido = redSocial.estaSucio();
    redSocial.asegurarActualizado();

    VerticeUsuario* vActivo = redSocial.buscarVertice(activo);
    VerticeUsuario* vObjetivo = redSocial.buscarVertice(usuario);

    if (vObjetivo == nullptr) {
        ostringstream err;
        err << "ERROR: el usuario " << usuario << " no existe en el grafo.";
        return err.str();
    }

    if (vActivo == nullptr) {
        ostringstream msg;
        msg << "Aun no tienes calificaciones registradas para calcular afinidad con "
            << vObjetivo->nombre << ".";
        return msg.str();
    }

    // Como el grafo solo guarda aristas con similitud > 0, nullptr indica Jaccard = 0.
    Arista* conexion = redSocial.buscarArista(activo, usuario);
    int pasosBusqueda = redSocial.pasosUltimoRecorrido();

    double similitud = 0.0;
    int interseccion = 0;
    int unionTotal = 0;

    if (conexion != nullptr) {
        similitud = conexion->similitud;
        interseccion = conexion->interseccion;
        unionTotal = conexion->unionTotal;
    } else {
        similitud = redSocial.calcularJaccard(activo, usuario, interseccion, unionTotal);
    }

    auto fin = chrono::high_resolution_clock::now();
    double tiempoMs = chrono::duration<double, milli>(fin - inicio).count();

    ostringstream o;
    o << fixed << setprecision(1);

    // 1. Diagnostico del Modo Desarrollador (Funcionalidad 6)
    bool dev = modoDevActivo() || modoDevLocal;
    if (dev) {
        o << "[DEV] Grafo Afinidad: " << redSocial.totalVertices() << " vertices, "
          << redSocial.totalAristas() << " aristas no dirigidas (similitud > 0)"
          << (reconstruido ? " [red reconstruida]" : " [red en cache]") << "\n";
        o << "[DEV] Adyacencia de U" << activo << ": "
          << redSocial.estadoAdyacenciaUsuario(activo) << "\n";
        o << "[DEV] Consulta U" << activo << "->U" << usuario << ": "
          << pasosBusqueda << " saltos de puntero | Jaccard (>=4 estrellas) |A n B|="
          << interseccion << ", |A u B|=" << unionTotal
          << " | Tiempo: " << setprecision(3) << tiempoMs << " ms\n"
          << setprecision(1);
    }

    // 2. Reporte de Similitud de Jaccard
    double porcentaje = similitud * 100.0;
    o << "Afinidad social con " << vObjetivo->nombre << ": " << porcentaje << "% "
      << "(Jaccard: " << interseccion << "/" << unionTotal << ")\n";

    // 3. Peliculas gustadas en comun (coincide con la interseccion de Jaccard)
    o << "Peliculas gustadas en comun (>= 4 estrellas) (" << interseccion << "):";
    if (interseccion == 0) {
        o << " ninguna\n";
    } else {
        o << "\n";
        NodoPelicula* pObj = vObjetivo->historial;
        while (pObj != nullptr) {
            if (pObj->estrellas >= UMBRAL_GUSTO &&
                redSocial.obtenerEstrellas(activo, pObj->idPelicula) >= UMBRAL_GUSTO) {
                o << "  * " << describirPelicula(pObj->idPelicula, false) << "\n";
            }
            pObj = pObj->sig;
        }
    }

    // 4. Recomendaciones colaborativas: gustadas por el otro usuario y no vistas por el activo
    o << "Recomendaciones colaborativas (vistas por " << vObjetivo->nombre << "):";
    int recomendadas = 0;
    NodoPelicula* pCand = vObjetivo->historial;
    while (pCand != nullptr) {
        if (pCand->estrellas >= UMBRAL_GUSTO &&
            !redSocial.usuarioVioPelicula(activo, pCand->idPelicula)) {
            recomendadas++;
            o << "\n  " << recomendadas << ". "
              << describirPelicula(pCand->idPelicula, true)
              << " - calificada con " << pCand->estrellas << " estrellas";
        }
        pCand = pCand->sig;
    }

    if (recomendadas == 0) {
        o << " ninguna pelicula nueva por recomendar.";
    }

    return o.str();
}