// Prueba aislada del modulo del Integrante 3 (Max-Heap + recomendador).
// No forma parte del programa final: tiene su propio main.
//
// Compilar desde la raiz del repositorio:
//   g++ -std=c++11 -Wall -Wextra -Isrc src/maxheap.cpp src/recomendador.cpp tests/prueba_recomendador.cpp -o prueba_recomendador
//   ./prueba_recomendador

#include "modulos.h"
#include "recomendador.h"
#include "maxheap.h"
#include <iostream>
#include <cstdlib>

using namespace std;

static int fallos = 0;

static void verificar(bool condicion, const string& nombre) {
    cout << (condicion ? "  [OK]    " : "  [FALLO] ") << nombre << "\n";
    if (!condicion) fallos++;
}

static void probarHeap() {
    cout << "--- MaxHeap ---\n";
    MaxHeap heap(2);                    // capacidad chica para forzar crecer()
    srand(7);
    for (int i = 0; i < 200; i++) {
        Candidato c = {i, (double)(rand() % 1000) / 10.0, 0, 0};
        heap.insertar(c);
    }
    verificar(heap.tamanio() == 200, "inserta 200 elementos y crece el arreglo");

    bool ordenado = true;
    Candidato anterior = heap.extraerMax();
    while (!heap.vacio()) {
        Candidato actual = heap.extraerMax();
        if (actual.puntaje > anterior.puntaje ||
            (actual.puntaje == anterior.puntaje && actual.id < anterior.id))
            ordenado = false;
        anterior = actual;
    }
    verificar(ordenado, "extraerMax sale en orden descendente (empate: id menor)");
    verificar(heap.vacio(), "queda vacio al final");
}

static void probarRecomendador() {
    cout << "--- Recomendador ---\n";
    verificar(explicar(104).find("ERROR") == 0, "explicar antes de recomendar da error");
    verificar(recomendar(0, "").find("ERROR") == 0, "top 0 da error");

    string top = recomendar(3, "");
    verificar(top.find("[116] Volver al Futuro") != string::npos, "top 3 incluye Volver al Futuro");
    verificar(top.find("[108]") == string::npos, "no recomienda peliculas ya calificadas");

    verificar(explicar(116).find("Puntaje total") != string::npos, "explicar una pelicula del top");
    verificar(explicar(105).find("ERROR") == 0, "explicar una pelicula fuera del top da error");

    string accion = recomendar(10, "accion");
    verificar(accion.find("solo hay 3 candidatas") != string::npos, "filtro de genero sin distinguir mayusculas");
    verificar(recomendar(3, "Western").find("No hay") == 0, "genero sin peliculas");
}

int main() {
    probarHeap();
    probarRecomendador();

    cout << "\n--- Demo (como lo veria el usuario) ---\n";
    recomendadorModoDev(true);
    cout << "> dev on\n> recomendar top 5\n" << recomendar(5, "") << "\n";
    recomendadorModoDev(false);
    cout << "> recomendar top 3 genero \"Ciencia Ficcion\"\n" << recomendar(3, "Ciencia Ficcion") << "\n";
    cout << "> explicar id 113\n" << explicar(113) << "\n";

    cout << "\n" << (fallos == 0 ? "TODAS LAS PRUEBAS PASARON" : "HAY PRUEBAS QUE FALLARON") << "\n";
    return fallos == 0 ? 0 : 1;
}
