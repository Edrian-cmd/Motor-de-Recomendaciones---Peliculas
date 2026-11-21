// Prueba aislada del modulo del Integrante 4 (Oliver: Grafo + Jaccard + Colaborativo).
// No forma parte del programa final: tiene su propio main y versiones falsas
// locales de las funciones del contrato v2 de los Integrantes 1, 2 y 5.
//
// Compilar desde la raiz del repositorio (requiere contrato v2 en src/modelo.h y src/modulos.h):
//   g++ -std=c++11 -Wall -Wextra -Isrc src/grafo.cpp src/afinidad.cpp tests/prueba_afinidad.cpp -o prueba_afinidad.exe
//   ./prueba_afinidad.exe

#include "modelo.h"
#include "modulos.h"
#include "afinidad.h"
#include "grafo.h"
#include <iostream>
#include <cmath>

using namespace std;

// ================= STUBS LOCALES DEL CONTRATO v2 (SOLO PARA ESTE TEST) =================

static const Pelicula CATALOGO_TEST[] = {
    {13, "Inception",          "Sci-Fi",    2010, 4.7, 10},
    {15, "The Matrix",         "Sci-Fi",    1999, 4.8, 12},
    {23, "Interstellar",       "Sci-Fi",    2014, 4.6, 8},
    {28, "Back to the Future", "Sci-Fi",    1985, 4.5, 9},
    {31, "Psycho",             "Terror",    1960, 4.2, 5},
    {70, "Coco",               "Animacion", 2017, 4.5, 7},
    {71, "Toy Story",          "Animacion", 1995, 4.4, 6}
};
static const int TOTAL_CATALOGO_TEST = sizeof(CATALOGO_TEST) / sizeof(CATALOGO_TEST[0]);

static int gUsuarioActivo = 1;
static bool gModoDev = false;

const Pelicula* obtenerPelicula(int id) {
    for (int i = 0; i < TOTAL_CATALOGO_TEST; i++) {
        if (CATALOGO_TEST[i].id == id)
            return &CATALOGO_TEST[i];
    }
    return nullptr;
}

int usuarioActivo() {
    return gUsuarioActivo;
}

bool modoDevActivo() {
    return gModoDev;
}

// ================= UTILIDADES DE PRUEBA =================

static int fallos = 0;

static void verificar(bool condicion, const string& nombre) {
    cout << (condicion ? "  [OK]    " : "  [FALLO] ") << nombre << "\n";
    if (!condicion) fallos++;
}

static bool casiIgual(double a, double b) {
    return fabs(a - b) < 1e-6;
}

// ================= PRUEBAS DE ESTRUCTURA (GrafoAfinidad) =================

static void probarGrafo() {
    cout << "--- GrafoAfinidad (Estructura, Jaccard >= 4 y Cache) ---\n";
    GrafoAfinidad g;

    verificar(g.insertarUsuario(1, "Usuario 1"), "inserta Usuario 1");
    verificar(g.insertarUsuario(2, "Usuario 2"), "inserta Usuario 2");
    verificar(g.insertarUsuario(3, "Usuario 3"), "inserta Usuario 3");
    verificar(!g.insertarUsuario(1, "Duplicado"), "rechaza ID duplicado");
    verificar(g.totalVertices() == 3, "conteo correcto de vertices (3)");
    verificar(g.estaSucio(), "marca grafo como sucio al insertar usuarios");

    // Insertar en orden desordenado para validar ordenamiento interno por idPelicula
    // Usuario 1: gustadas (>= 4) = {101, 103, 105}; 102 tiene 2 estrellas (no cuenta)
    g.agregarPelicula(1, 105, 5);
    g.agregarPelicula(1, 101, 5);
    g.agregarPelicula(1, 102, 2);
    g.agregarPelicula(1, 103, 4);

    // Usuario 2: gustadas (>= 4) = {103, 104, 105}; 102 tiene 1 estrella (no cuenta)
    g.agregarPelicula(2, 104, 5);
    g.agregarPelicula(2, 102, 1);
    g.agregarPelicula(2, 105, 4);
    g.agregarPelicula(2, 103, 5);

    // Usuario 3: gustadas (>= 4) = {106, 107} (sin coincidencias con 1 ni 2)
    g.agregarPelicula(3, 107, 4);
    g.agregarPelicula(3, 106, 5);

    // Interseccion gustadas(1, 2) = {103, 105} (2), Union = {101, 103, 104, 105} (4) -> 0.5
    int inter = 0, uni = 0;
    double j12 = g.calcularJaccard(1, 2, inter, uni);
    verificar(inter == 2 && uni == 4, "Jaccard ignora calificaciones < 4 (inter=2, union=4)");
    verificar(casiIgual(j12, 0.5), "Jaccard por mezcla lineal O(|A|+|B|) da 0.5 exacto");

    g.asegurarActualizado();
    verificar(!g.estaSucio(), "asegurarActualizado reconstruye y limpia bandera sucio");
    verificar(g.totalAristas() == 1, "grafo disperso solo crea aristas con similitud > 0");
    verificar(g.buscarArista(1, 2) != nullptr && g.buscarArista(2, 1) != nullptr,
              "arista bidireccional (1 <-> 2) creada");
    verificar(g.buscarArista(1, 3) == nullptr, "no crea arista cuando similitud es 0");

    // Probar eliminacion (convencion deshacer)
    verificar(g.eliminarPelicula(1, 105), "eliminarPelicula quita nodo existente");
    verificar(!g.usuarioVioPelicula(1, 105), "pelicula eliminada ya no aparece en historial");
    verificar(g.estaSucio(), "eliminarPelicula vuelve a marcar grafo como sucio");

    // Ahora U1 gustadas = {101, 103}, U2 gustadas = {103, 104, 105} -> Jaccard = 1/4 = 0.25
    g.asegurarActualizado();
    Arista* a12 = g.buscarArista(1, 2);
    verificar(a12 != nullptr && casiIgual(a12->similitud, 0.25),
              "reconstruccion actualiza similitud a 0.25 tras eliminar");

    // Probar puntaje colaborativo ponderado y cache de vecinos
    // U4 gustadas = {101, 103} -> Jaccard(1, 4) = 2/2 = 1.0; ademas califico 104 con 3 estrellas
    g.insertarUsuario(4, "Usuario 4");
    g.agregarPelicula(4, 101, 5);
    g.agregarPelicula(4, 103, 5);
    g.agregarPelicula(4, 104, 3);

    // Para pelicula 104:
    //   U4: sim = 1.0, estrellas = 3 -> norm = (3-1)/4 = 0.5
    //   U2: sim = 0.25, estrellas = 5 -> norm = (5-1)/4 = 1.0
    //   Esperado = (1.0*0.5 + 0.25*1.0) / (1.0 + 0.25) = 0.75 / 1.25 = 0.6
    double p104 = g.calcularPuntajeColaborativo(1, 104, 10);
    verificar(casiIgual(p104, 0.6), "puntajeColaborativo pondera vecinos correctamente (0.6)");
    verificar(g.tamanoCacheVecinos() == 2, "cache guarda los 2 vecinos con similitud > 0");

    // Para pelicula 105: solo U2 la califico con 4 estrellas -> norm = (4-1)/4 = 0.75
    double p105 = g.calcularPuntajeColaborativo(1, 105, 10);
    verificar(casiIgual(p105, 0.75), "puntajeColaborativo usa cache caliente para otra pelicula (0.75)");

    // Para pelicula no vista por ningun vecino -> 0.0
    verificar(casiIgual(g.calcularPuntajeColaborativo(1, 999, 10), 0.0),
              "puntajeColaborativo devuelve 0.0 si ningun vecino la califico");
}

// ================= PRUEBAS DE CONTRATO v2 (afinidad.cpp) =================

static void probarContrato() {
    cout << "--- Modulo Afinidad (Contrato v2 sin mocks) ---\n";

    gUsuarioActivo = 1;
    gModoDev = false;

    // Usuario 1 (activo): gustadas {13, 15}; 31 con 1 estrella
    grafoRegistrarCalificacion(1, 13, 5);
    grafoRegistrarCalificacion(1, 15, 5);
    grafoRegistrarCalificacion(1, 31, 1);

    // Usuario 2 (fan Sci-Fi): gustadas {13, 15, 23, 28}; 31 con 2 estrellas
    // Jaccard(1, 2) = 2/4 = 50.0%
    grafoRegistrarCalificacion(2, 13, 5);
    grafoRegistrarCalificacion(2, 15, 4);
    grafoRegistrarCalificacion(2, 23, 5);
    grafoRegistrarCalificacion(2, 28, 4);
    grafoRegistrarCalificacion(2, 31, 2);

    // Usuario 3 (fan Animacion): gustadas {70, 71} -> Jaccard(1, 3) = 0/4 = 0.0%
    grafoRegistrarCalificacion(3, 70, 5);
    grafoRegistrarCalificacion(3, 71, 5);

    // Usuario 4: incluye ID 999 fuera de CATALOGO_TEST para probar manejo de nullptr
    grafoRegistrarCalificacion(4, 13, 5);
    grafoRegistrarCalificacion(4, 999, 5);

    // Validaciones de errores
    verificar(afinidad(0).find("ERROR") == 0, "usuario 0 devuelve ERROR");
    verificar(afinidad(1).find("ERROR") == 0, "comparar con uno mismo devuelve ERROR");
    verificar(afinidad(99).find("ERROR") == 0, "usuario inexistente devuelve ERROR");

    // Reporte con afinidad positiva
    string rep2 = afinidad(2);
    verificar(rep2.find("50.0% (Jaccard: 2/4)") != string::npos,
              "afinidad(2) calcula 50.0% (Jaccard: 2/4)");
    verificar(rep2.find("[13] Inception") != string::npos &&
              rep2.find("[15] The Matrix") != string::npos,
              "afinidad(2) muestra peliculas gustadas en comun usando obtenerPelicula()");
    verificar(rep2.find("[23] Interstellar") != string::npos &&
              rep2.find("[28] Back to the Future") != string::npos,
              "afinidad(2) recomienda peliculas gustadas por U2 y no vistas por U1");
    verificar(rep2.find("[31]") == string::npos,
              "afinidad(2) no incluye peliculas con calificacion < 4");

    // Reporte con 0% de afinidad
    string rep3 = afinidad(3);
    verificar(rep3.find("0.0% (Jaccard: 0/4)") != string::npos,
              "afinidad(3) funciona sin arista y reporta 0.0%");

    // Respaldo seguro cuando obtenerPelicula devuelve nullptr
    string rep4 = afinidad(4);
    verificar(rep4.find("[999] Pelicula #999") != string::npos,
              "maneja obtenerPelicula() == nullptr sin fallar");

    // Puntaje colaborativo del contrato para el recomendador de Piero
    verificar(casiIgual(puntajeColaborativo(23), 1.0),
              "puntajeColaborativo(23) devuelve 1.0 (vecino U2 dio 5 estrellas)");
    verificar(casiIgual(puntajeColaborativo(28), 0.75),
              "puntajeColaborativo(28) devuelve 0.75 (vecino U2 dio 4 estrellas)");
    verificar(casiIgual(puntajeColaborativo(70), 0.0),
              "puntajeColaborativo(70) devuelve 0.0 (U3 tiene afinidad 0)");

    // Convencion 0 estrellas (deshacer de Imanol)
    grafoRegistrarCalificacion(1, 15, 0);
    string rep2Deshacer = afinidad(2);
    verificar(rep2Deshacer.find("25.0% (Jaccard: 1/4)") != string::npos,
              "grafoRegistrarCalificacion(..., 0) quita pelicula y recalcula a 25.0%");

    // Restaurar para probar modo dev y demo
    grafoRegistrarCalificacion(1, 15, 5);

    // Modo desarrollador a traves de modoDevActivo()
    gModoDev = true;
    verificar(afinidad(2).find("[DEV]") != string::npos,
              "modoDevActivo() == true incluye diagnostico [DEV]");
    gModoDev = false;
    verificar(afinidad(2).find("[DEV]") == string::npos,
              "modoDevActivo() == false oculta diagnostico [DEV]");

    // Cambio de usuario activo a uno sin calificaciones
    gUsuarioActivo = 50;
    verificar(afinidad(2).find("Aun no tienes calificaciones") != string::npos,
              "usuario activo sin calificaciones devuelve mensaje claro");
    verificar(casiIgual(puntajeColaborativo(23), 0.0),
              "puntajeColaborativo devuelve 0.0 para usuario sin calificaciones");
    gUsuarioActivo = 1;
}

int main() {
    probarGrafo();
    probarContrato();

    cout << "\n--- Demo (como lo veria el usuario en el CMD) ---\n";
    gModoDev = true;
    cout << "> dev on\n> afinidad usuario 2\n" << afinidad(2) << "\n\n";

    gModoDev = false;
    cout << "> dev off\n> afinidad usuario 3\n" << afinidad(3) << "\n\n";
    cout << "> puntajeColaborativo(23) = " << puntajeColaborativo(23) << "\n";
    cout << "> puntajeColaborativo(28) = " << puntajeColaborativo(28) << "\n";

    cout << "\n" << (fallos == 0 ? "TODAS LAS PRUEBAS PASARON" : "HAY PRUEBAS QUE FALLARON") << "\n";
    return fallos == 0 ? 0 : 1;
}