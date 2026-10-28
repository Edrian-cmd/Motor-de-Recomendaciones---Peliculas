// Prueba aislada del modulo del Integrante 4 (Grafo + Similitud de Jaccard).
// No forma parte del programa final: tiene su propio main.
//
// Compilar desde la raiz del repositorio:
//   g++ -std=c++11 -Wall -Wextra -Isrc src/grafo.cpp src/afinidad.cpp tests/prueba_afinidad.cpp -o prueba_afinidad
//   ./prueba_afinidad

#include "modulos.h"
#include "afinidad.h"
#include "grafo.h"
#include <iostream>
#include <cmath>

using namespace std;

static int fallos = 0;

static void verificar(bool condicion, const string& nombre) {
    cout << (condicion ? "  [OK]    " : "  [FALLO] ") << nombre << "\n";
    if (!condicion) fallos++;
}

static void probarGrafo() {
    cout << "--- GrafoAfinidad (Estructura y Jaccard) ---\n";
    GrafoAfinidad g;

    verificar(g.insertarUsuario(1, "UserA"), "inserta Usuario 1 en memoria dinamica");
    verificar(g.insertarUsuario(2, "UserB"), "inserta Usuario 2 en memoria dinamica");
    verificar(g.insertarUsuario(3, "UserC"), "inserta Usuario 3 en memoria dinamica");
    verificar(!g.insertarUsuario(1, "Repetido"), "rechaza insertar un ID de usuario duplicado");
    verificar(g.totalVertices() == 3, "conteo correcto de vertices (3)");

    // User 1 vio: 101, 102, 103
    g.agregarPelicula(1, 101, 5);
    g.agregarPelicula(1, 102, 4);
    g.agregarPelicula(1, 103, 3);

    // User 2 vio: 102, 103, 104 (Comparte 2 de un total de 4 unicas -> Jaccard = 2/4 = 0.5)
    g.agregarPelicula(2, 102, 5);
    g.agregarPelicula(2, 103, 4);
    g.agregarPelicula(2, 104, 5);

    int inter = 0, uni = 0;
    double j = g.calcularJaccard(1, 2, inter, uni);
    verificar(inter == 2 && uni == 4, "calcula interseccion (2) y union (4) correctamente");
    verificar(fabs(j - 0.5) < 1e-6, "formula de Similitud de Jaccard da 0.5 exacto");

    g.construirRedAfinidad();
    verificar(g.totalAristas() == 3, "construye las 3 aristas no dirigidas entre 3 usuarios");

    Arista* a12 = g.buscarArista(1, 2);
    Arista* a21 = g.buscarArista(2, 1);
    verificar(a12 != nullptr && a21 != nullptr && fabs(a12->similitud - 0.5) < 1e-6,
              "arista bidireccional (1<->2) creada con peso Jaccard correcto");
}

static void probarAfinidad() {
    cout << "--- Modulo Afinidad (Contrato modulos.h) ---\n";
    verificar(afinidad(0).find("ERROR") == 0, "usuario 0 o negativo devuelve ERROR");
    verificar(afinidad(99).find("ERROR") == 0, "usuario inexistente devuelve ERROR");

    string rep1 = afinidad(1);
    verificar(rep1.find("44.4% (Jaccard: 4/9)") != string::npos,
              "afinidad con Usuario 1 calcula 44.4% (4 en comun de 9)");
    verificar(rep1.find("[116] Volver al Futuro") != string::npos,
              "recomienda peliculas que Usuario 1 califico bien y el activo no vio");

    string rep4 = afinidad(4);
    verificar(rep4.find("0.0% (Jaccard: 0/8)") != string::npos,
              "afinidad con Usuario 4 da 0.0% cuando no hay peliculas en comun");
}

int main() {
    probarGrafo();
    probarAfinidad();

    cout << "\n--- Demo (como lo veria el usuario en el CMD) ---\n";
    afinidadModoDev(true);
    cout << "> dev on\n> afinidad 1\n" << afinidad(1) << "\n\n";

    afinidadModoDev(false);
    cout << "> dev off\n> afinidad 2\n" << afinidad(2) << "\n\n";
    cout << "> afinidad 4\n" << afinidad(4) << "\n";

    cout << "\n" << (fallos == 0 ? "TODAS LAS PRUEBAS PASARON" : "HAY PRUEBAS QUE FALLARON") << "\n";
    return fallos == 0 ? 0 : 1;
}