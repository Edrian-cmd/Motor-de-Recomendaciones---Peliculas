// Prueba aislada del modulo del Integrante 3 (Max-Heap + recomendador).
// No forma parte del programa final: tiene su propio main.
//
// El recomendador depende de funciones de otros integrantes (contrato v2).
// Para probarlo SOLO, este archivo trae versiones falsas ("dobles de prueba")
// de esas funciones con un catalogo y un perfil pequenos escritos a mano.
// No se usan en el programa real: alli las ponen el Hash (Int.1), el perfil
// (Int.2), el grafo (Int.4) y el interprete (Int.5).
//
// Compilar desde la raiz del repositorio:
//   g++ -std=c++11 -Wall -Wextra -Isrc/contrato -Isrc/recomendador src/recomendador/maxheap.cpp src/recomendador/recomendador.cpp tests/prueba_recomendador.cpp -o prueba_recomendador   (o: compilar.bat pruebas)
//   ./prueba_recomendador

#include "modulos.h"
#include "modelo.h"
#include "recomendador.h"
#include "maxheap.h"
#include <iostream>
#include <cstdlib>

using namespace std;

// ================= DOBLES DE PRUEBA (contrato v2) =================

static Pelicula CATALOGO_PRUEBA[] = {
    // id, titulo, genero, anio, ratingPromedio, numVotos
    {1,  "Matrix",            "Sci-Fi",    1999, 4.7, 120},
    {2,  "Inception",         "Sci-Fi",    2010, 4.6, 200},
    {3,  "Interstellar",      "Sci-Fi",    2014, 4.8,   1},  // nota alta, 1 solo voto
    {4,  "El Conjuro",        "Terror",    2013, 4.0,  80},
    {5,  "El Resplandor",     "Terror",    1980, 4.5, 150},
    {6,  "Superbad",          "Comedia",   2007, 3.7,  60},
    {7,  "Toy Story",         "Animacion", 1995, 4.5, 100},
    {8,  "Coco",              "Animacion", 2017, 4.5, 100},  // igual a Toy Story + colaborativo
    {9,  "Blade Runner 2049", "Sci-Fi",    2017, 4.2,  40},
    {10, "John Wick",         "Accion",    2014, 4.1,  70},
};
static const int TOTAL_PRUEBA = sizeof(CATALOGO_PRUEBA) / sizeof(CATALOGO_PRUEBA[0]);

// Usuario activo: le gusta Sci-Fi y odia el terror.
static Calificacion perfilPrueba[] = {
    {1, 1, 5}, {1, 9, 4}, {1, 4, 1},
};
static int totalPerfil = 3;
static bool devPrueba = false;

// ---- Int.1 ----
const Pelicula* obtenerPelicula(int id) {
    for (int i = 0; i < TOTAL_PRUEBA; i++)
        if (CATALOGO_PRUEBA[i].id == id) return &CATALOGO_PRUEBA[i];
    return nullptr;
}
int totalPeliculas() { return TOTAL_PRUEBA; }
const Pelicula* peliculaPorIndice(int i) {
    return (i >= 0 && i < TOTAL_PRUEBA) ? &CATALOGO_PRUEBA[i] : nullptr;
}

// ---- Int.2 ----
int estrellasDelActivo(int idPelicula) {
    for (int i = 0; i < totalPerfil; i++)
        if (perfilPrueba[i].idPelicula == idPelicula) return perfilPrueba[i].estrellas;
    return 0;
}
int calificacionesDelActivo(Calificacion* destino, int max) {
    int k = 0;
    for (int i = 0; i < totalPerfil && k < max; i++)
        destino[k++] = perfilPrueba[i];
    return k;
}

// ---- Int.4 ----
double puntajeColaborativo(int idPelicula) {
    return idPelicula == 8 ? 0.6 : 0.0;      // los vecinos recomiendan Coco
}

// ---- Int.5 ----
bool modoDevActivo() { return devPrueba; }

// ================= PRUEBAS =================

static int fallos = 0;

static void verificar(bool condicion, const string& nombre) {
    cout << (condicion ? "  [OK]    " : "  [FALLO] ") << nombre << "\n";
    if (!condicion) fallos++;
}

// true si "a" aparece antes que "b" dentro de texto
static bool antes(const string& texto, const string& a, const string& b) {
    size_t pa = texto.find(a), pb = texto.find(b);
    return pa != string::npos && pb != string::npos && pa < pb;
}

static void probarHeap() {
    cout << "--- MaxHeap ---\n";
    MaxHeap heap(2);                    // capacidad chica para forzar crecer()
    srand(7);
    for (int i = 0; i < 200; i++) {
        Candidato c = {i, (double)(rand() % 1000) / 10.0, 0, 0, 0, 0, 0};
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
    verificar(explicar(2).find("ERROR") == 0, "explicar antes de recomendar da error");
    verificar(recomendar(0, "").find("ERROR") == 0, "top 0 da error");

    string top = recomendar(10, "");
    verificar(top.find("solo hay 7 candidatas") != string::npos, "10 peliculas - 3 calificadas = 7 candidatas");
    verificar(top.find("[1]") == string::npos && top.find("[4]") == string::npos &&
              top.find("[9]") == string::npos, "no recomienda peliculas ya calificadas");
    verificar(top.find("1. [2] Inception") != string::npos, "el primero es Sci-Fi (genero favorito)");
    verificar(antes(top, "[2]", "[3]"), "promedio bayesiano: 200 votos de 4.6 le ganan a 1 voto de 4.8");
    verificar(antes(top, "[8]", "[7]"), "colaborativo: Coco supera a Toy Story (iguales salvo el grafo)");
    verificar(top.find("7. [5] El Resplandor") != string::npos, "terror (genero odiado) queda ultimo");

    string expl = explicar(2);
    verificar(expl.find("afinidad +0.75") != string::npos, "explicar muestra la afinidad de genero");
    verificar(expl.find("Componente colaborativo") != string::npos &&
              expl.find("Puntaje total") != string::npos, "explicar desglosa los 4 componentes");
    verificar(explicar(1).find("ERROR") == 0, "explicar una pelicula fuera del top da error");

    verificar(recomendar(10, "sci-fi").find("solo hay 2 candidatas") != string::npos,
              "filtro de genero sin distinguir mayusculas");
    verificar(recomendar(3, "Western").find("No hay") == 0, "genero sin peliculas");

    devPrueba = true;
    verificar(recomendar(3, "").find("[DEV] Max-Heap") != string::npos, "modo dev global muestra el heap");
    devPrueba = false;
    verificar(recomendar(3, "").find("[DEV]") == string::npos, "sin modo dev no hay diagnostico");
}

static void probarTopN() {
    cout << "--- topN (para la cola de descubrir del Int.2) ---\n";
    int ids[20];
    int k = topN(20, ids);
    verificar(k == 7, "topN devuelve solo las 7 candidatas");
    verificar(k > 0 && ids[0] == 2, "topN sale en el mismo orden que recomendar");
    bool sinCalificadas = true;
    for (int i = 0; i < k; i++)
        if (ids[i] == 1 || ids[i] == 4 || ids[i] == 9) sinCalificadas = false;
    verificar(sinCalificadas, "topN excluye las calificadas");
    verificar(topN(0, ids) == 0 && topN(3, nullptr) == 0, "topN con n=0 o sin destino devuelve 0");
}

static void probarUsuarioNuevo() {
    cout << "--- Usuario sin calificaciones (arranque en frio) ---\n";
    int guardado = totalPerfil;
    totalPerfil = 0;
    string top = recomendar(3, "");
    verificar(top.find("Top 3 recomendaciones:") == 0, "recomienda aunque no haya calificado nada");
    string todo = recomendar(10, "");
    verificar(todo.find("Top 10 recomendaciones:") == 0 && todo.find("solo hay") == string::npos,
              "sin calificaciones las 10 peliculas son candidatas");
    verificar(antes(todo, "[2]", "[3]"), "sin perfil decide calidad bayesiana + popularidad");
    totalPerfil = guardado;
}

int main() {
    probarHeap();
    probarRecomendador();
    probarTopN();
    probarUsuarioNuevo();

    cout << "\n--- Demo (como lo veria el usuario) ---\n";
    devPrueba = true;
    cout << "> dev on\n> recomendar top 5\n" << recomendar(5, "") << "\n";
    devPrueba = false;
    cout << "> explicar id 8\n" << explicar(8) << "\n";

    cout << "\n" << (fallos == 0 ? "TODAS LAS PRUEBAS PASARON" : "HAY PRUEBAS QUE FALLARON") << "\n";
    return fallos == 0 ? 0 : 1;
}
