#include "modulos.h"
#include "afinidad.h"
#include "grafo.h"
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace std;

// ================= DATOS DE PRUEBA (MOCK) =================
// TEMPORAL (Entregable 2): Mismo catalogo y usuario activo que el Integrante 3
// mas 4 usuarios simulados para poblar el Grafo de Afinidad Social.
// En el Entregable 3 se conectara con el Hash (Int.1) y Perfiles (Int.2).

struct PeliculaMock {
    int id;
    string titulo;
    string genero;
    int anio;
};

struct CalificacionMock {
    int idPelicula;
    int estrellas; // 1 a 5
};

static const PeliculaMock CATALOGO[] = {
    {101, "El Padrino",                  "Drama",           1972},
    {102, "Mad Max: Furia en el Camino", "Accion",          2015},
    {103, "Toy Story",                   "Animacion",       1995},
    {104, "Interestelar",                "Ciencia Ficcion", 2014},
    {105, "El Conjuro",                  "Terror",          2013},
    {106, "Superbad",                    "Comedia",         2007},
    {107, "Duro de Matar",               "Accion",          1988},
    {108, "Matrix",                      "Ciencia Ficcion", 1999},
    {109, "Forrest Gump",                "Drama",           1994},
    {110, "Coco",                        "Animacion",       2017},
    {111, "John Wick",                   "Accion",          2014},
    {112, "El Resplandor",               "Terror",          1980},
    {113, "Blade Runner 2049",           "Ciencia Ficcion", 2017},
    {114, "Que Paso Ayer",               "Comedia",         2009},
    {115, "Gladiador",                   "Accion",          2000},
    {116, "Volver al Futuro",            "Ciencia Ficcion", 1985},
};
static const int TOTAL_PELICULAS = sizeof(CATALOGO) / sizeof(CATALOGO[0]);

// ID 0 reservado para el Usuario Activo (mismo perfil que en recomendador.cpp)
static const int ID_USUARIO_ACTIVO = 0;
static const CalificacionMock PERFIL_ACTIVO[] = {
    {108, 5}, {104, 5}, {107, 4}, {105, 1}, {106, 2}, {101, 3}
};

// Perfiles de otros usuarios de la comunidad para el Grafo
static const CalificacionMock PERFIL_U1[] = { // Alta afinidad (Sci-Fi / Accion)
    {108, 5}, {104, 5}, {107, 5}, {101, 4}, {116, 5}, {113, 4}, {102, 4}
};
static const CalificacionMock PERFIL_U2[] = { // Media afinidad (Drama / Mixto)
    {104, 4}, {101, 5}, {106, 3}, {109, 5}, {115, 4}, {103, 2}
};
static const CalificacionMock PERFIL_U3[] = { // Baja afinidad (Terror / Comedia)
    {105, 5}, {106, 4}, {112, 5}, {114, 4}
};
static const CalificacionMock PERFIL_U4[] = { // Sin peliculas en comun (0% afinidad)
    {103, 5}, {110, 5}
};

// ================= ESTADO DEL MODULO =================

static bool modoDev = false;
static GrafoAfinidad redSocial;
static bool grafoInicializado = false;

void afinidadModoDev(bool activo) {
    modoDev = activo;
}

// ================= AUXILIARES =================

static const PeliculaMock* obtenerPelicula(int id) {
    for (int i = 0; i < TOTAL_PELICULAS; i++) {
        if (CATALOGO[i].id == id)
            return &CATALOGO[i];
    }
    return nullptr;
}

static void cargarHistorialMock(int idUsuario, const CalificacionMock* lista, int n) {
    for (int i = 0; i < n; i++) {
        redSocial.agregarPelicula(idUsuario, lista[i].idPelicula, lista[i].estrellas);
    }
}

static void inicializarGrafoMock() {
    if (grafoInicializado) return;

    redSocial.insertarUsuario(ID_USUARIO_ACTIVO, "Usuario Activo (Tu)");
    redSocial.insertarUsuario(1, "Ana (Fan Sci-Fi)");
    redSocial.insertarUsuario(2, "Carlos (Cinefilo Clasico)");
    redSocial.insertarUsuario(3, "Elena (Fan Terror/Comedia)");
    redSocial.insertarUsuario(4, "Diego (Fan Animacion)");

    cargarHistorialMock(ID_USUARIO_ACTIVO, PERFIL_ACTIVO,
                        sizeof(PERFIL_ACTIVO) / sizeof(PERFIL_ACTIVO[0]));
    cargarHistorialMock(1, PERFIL_U1, sizeof(PERFIL_U1) / sizeof(PERFIL_U1[0]));
    cargarHistorialMock(2, PERFIL_U2, sizeof(PERFIL_U2) / sizeof(PERFIL_U2[0]));
    cargarHistorialMock(3, PERFIL_U3, sizeof(PERFIL_U3) / sizeof(PERFIL_U3[0]));
    cargarHistorialMock(4, PERFIL_U4, sizeof(PERFIL_U4) / sizeof(PERFIL_U4[0]));

    redSocial.construirRedAfinidad();
    grafoInicializado = true;
}

// ================= CONTRATO (modulos.h) =================

string afinidad(int usuario) {
    if (usuario <= 0)
        return "ERROR: el ID de usuario a comparar debe ser mayor que 0.";

    auto inicio = chrono::high_resolution_clock::now();

    inicializarGrafoMock();

    VerticeUsuario* vActivo = redSocial.buscarVertice(ID_USUARIO_ACTIVO);
    VerticeUsuario* vObjetivo = redSocial.buscarVertice(usuario);

    if (vObjetivo == nullptr) {
        ostringstream err;
        err << "ERROR: el usuario " << usuario
            << " no existe en el grafo (usuarios disponibles: 1 al 4).";
        return err.str();
    }

    Arista* conexion = redSocial.buscarArista(ID_USUARIO_ACTIVO, usuario);
    int pasosBusqueda = redSocial.pasosUltimoRecorrido();

    auto fin = chrono::high_resolution_clock::now();
    double tiempoMs = chrono::duration<double, milli>(fin - inicio).count();

    ostringstream o;
    o << fixed << setprecision(1);

    // 1. Diagnostico del Modo Desarrollador (Funcionalidad 6)
    if (modoDev) {
        o << "[DEV] Grafo Afinidad: " << redSocial.totalVertices() << " vertices, "
          << redSocial.totalAristas() << " aristas no dirigidas\n";
        o << "[DEV] Listas de adyacencia: " << redSocial.estadoAdyacencia() << "\n";
        o << "[DEV] Recorrido de busqueda U0->U" << usuario << ": "
          << pasosBusqueda << " saltos de puntero | Jaccard |A n B|="
          << conexion->interseccion << ", |A u B|=" << conexion->unionTotal
          << " | Tiempo: " << setprecision(3) << tiempoMs << " ms\n"
          << setprecision(1);
    }

    // 2. Reporte de Similitud de Jaccard para el usuario
    double porcentaje = conexion->similitud * 100.0;
    o << "Afinidad social con Usuario " << vObjetivo->id
      << " (" << vObjetivo->nombre << "): " << porcentaje << "% "
      << "(Jaccard: " << conexion->interseccion << "/" << conexion->unionTotal << ")\n";

    // 3. Mostrar peliculas en comun (Interseccion |A n B|)
    o << "Peliculas en comun (" << conexion->interseccion << "):";
    if (conexion->interseccion == 0) {
        o << " ninguna\n";
    } else {
        o << "\n";
        NodoPelicula* pObj = vObjetivo->historial;
        while (pObj != nullptr) {
            if (redSocial.usuarioVioPelicula(ID_USUARIO_ACTIVO, pObj->idPelicula)) {
                const PeliculaMock* info = obtenerPelicula(pObj->idPelicula);
                if (info != nullptr) {
                    o << "  * [" << info->id << "] " << info->titulo
                      << " (" << info->genero << ")\n";
                }
            }
            pObj = pObj->sig;
        }
    }

    // 4. Recomendacion Colaborativa: peliculas que el otro usuario califico bien (>= 4)
    //    y que el Usuario Activo aun NO ha visto.
    o << "Recomendaciones colaborativas (vistas por Usuario " << usuario << "):";
    int recomendadas = 0;
    NodoPelicula* pCand = vObjetivo->historial;
    while (pCand != nullptr) {
        if (pCand->estrellas >= 4 &&
            !redSocial.usuarioVioPelicula(ID_USUARIO_ACTIVO, pCand->idPelicula)) {
            const PeliculaMock* info = obtenerPelicula(pCand->idPelicula);
            if (info != nullptr) {
                recomendadas++;
                o << "\n  " << recomendadas << ". [" << info->id << "] "
                  << info->titulo << " (" << info->anio << ") - " << info->genero
                  << " - calificada con " << pCand->estrellas << " estrellas";
            }
        }
        pCand = pCand->sig;
    }

    if (recomendadas == 0) {
        o << " ninguna pelicula nueva por recomendar.";
    }

    return o.str();
}