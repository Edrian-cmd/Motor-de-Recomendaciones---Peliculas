#include "modulos.h"
#include "modelo.h"
#include "avl.h"
#include "hash.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;

// Arreglo dinamico de punteros Pelicula* en orden de carga (sin usar std::vector)
struct ArregloCatalogo {
    Pelicula** datos;
    int cantidad;
    int capacidad;

    ArregloCatalogo() : datos(nullptr), cantidad(0), capacidad(0) {}

    ~ArregloCatalogo() {
        limpiar();
    }

    void limpiar() {
        if (datos != nullptr) {
            delete[] datos;
            datos = nullptr;
        }
        cantidad = 0;
        capacidad = 0;
    }

    // Insercion amortizada en O(1) duplicando capacidad cuando se llena
    void agregar(Pelicula* p) {
        if (p == nullptr) return;
        if (cantidad == capacidad) {
            int nuevaCapacidad = (capacidad == 0) ? 16 : capacidad * 2;
            Pelicula** nuevoArreglo = new Pelicula*[nuevaCapacidad];
            for (int i = 0; i < cantidad; i++) {
                nuevoArreglo[i] = datos[i];
            }
            delete[] datos;
            datos = nuevoArreglo;
            capacidad = nuevaCapacidad;
        }
        datos[cantidad] = p;
        cantidad++;
    }
};

static TablaHash miCatalogoHash;
static ArbolAVL miCatalogoAVL;
static ArregloCatalogo miCatalogoLineal;

static string limpiarRetornoCarro(const string& texto) {
    if (!texto.empty() && texto.back() == '\r') {
        return texto.substr(0, texto.size() - 1);
    }
    return texto;
}

bool cargarCatalogo(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        return false;
    }

    // Limpiamos estructuras por si se recarga el catalogo
    miCatalogoLineal.limpiar();
    miCatalogoHash.limpiar();
    miCatalogoAVL.limpiar();

    string linea;
    while (getline(archivo, linea)) {
        linea = limpiarRetornoCarro(linea);
        if (linea.empty()) continue;

        stringstream ss(linea);
        string id_str, titulo, genero, anio_str;

        getline(ss, id_str, ',');
        getline(ss, titulo, ',');
        getline(ss, genero, ',');
        getline(ss, anio_str, ',');

        titulo = limpiarRetornoCarro(titulo);
        genero = limpiarRetornoCarro(genero);
        anio_str = limpiarRetornoCarro(anio_str);

        try {
            int id = stoi(id_str);
            int anio = stoi(anio_str);

            Pelicula* insertada = miCatalogoHash.insertar(id, titulo, genero, anio);
            if (insertada != nullptr) {
                miCatalogoLineal.agregar(insertada);
                miCatalogoAVL.insertar(id, titulo, genero, anio);
            }
        } catch (...) {
            // Ignora la primera linea de encabezados (ID,Titulo,Genero,Anio)
        }
    }

    archivo.close();
    return true;
}

string buscarPorId(int id) {
    auto inicio = chrono::high_resolution_clock::now();
    string respuesta = miCatalogoHash.buscar(id);
    auto fin = chrono::high_resolution_clock::now();

    if (modoDevActivo()) {
        double ms = chrono::duration<double, milli>(fin - inicio).count();
        ostringstream dev;
        dev << "\n[DEV] TablaHash | Sondeos busqueda: " << miCatalogoHash.obtenerSondeosUltimaBusqueda()
            << " | Colisiones carga: " << miCatalogoHash.obtenerColisionesTotales()
            << " | Tiempo: " << fixed << setprecision(4) << ms << " ms";
        respuesta += dev.str();
    }
    return respuesta;
}

string buscarPorGenero(const string& genero) {
    auto inicio = chrono::high_resolution_clock::now();
    string respuesta = miCatalogoAVL.buscarPorGenero(genero);
    auto fin = chrono::high_resolution_clock::now();

    if (modoDevActivo()) {
        double ms = chrono::duration<double, milli>(fin - inicio).count();
        ostringstream dev;
        dev << "\n[DEV] ArbolAVL (In-Order) | Altura: " << miCatalogoAVL.obtenerAlturaRaiz()
            << " | Rotaciones carga: " << miCatalogoAVL.obtenerRotacionesTotales()
            << " | Nodos visitados: " << miCatalogoAVL.obtenerNodosVisitados()
            << " | Tiempo: " << fixed << setprecision(4) << ms << " ms";
        respuesta += dev.str();
    }
    return respuesta;
}

string buscarPorAnio(int desde, int hasta) {
    // "buscar anio 2010..2000" se interpreta igual que 2000..2010
    if (desde > hasta) {
        int tmp = desde;
        desde = hasta;
        hasta = tmp;
    }
    auto inicio = chrono::high_resolution_clock::now();
    string respuesta = miCatalogoAVL.buscarPorAnio(desde, hasta);
    auto fin = chrono::high_resolution_clock::now();

    if (modoDevActivo()) {
        double ms = chrono::duration<double, milli>(fin - inicio).count();
        ostringstream dev;
        dev << "\n[DEV] ArbolAVL (Rango con poda) | Altura: " << miCatalogoAVL.obtenerAlturaRaiz()
            << " | Rotaciones carga: " << miCatalogoAVL.obtenerRotacionesTotales()
            << " | Nodos visitados: " << miCatalogoAVL.obtenerNodosVisitados()
            << " | Tiempo: " << fixed << setprecision(4) << ms << " ms";
        respuesta += dev.str();
    }
    return respuesta;
}

// Busca y retorna puntero constante a la Pelicula en O(1) promedio
const Pelicula* obtenerPelicula(int id) {
    return miCatalogoHash.obtener(id);
}

// Total de peliculas cargadas en O(1)
int totalPeliculas() {
    return miCatalogoLineal.cantidad;
}

// Acceso por indice (0 .. totalPeliculas()-1) en O(1) para recorrer el catalogo
const Pelicula* peliculaPorIndice(int i) {
    if (i < 0 || i >= miCatalogoLineal.cantidad) {
        return nullptr;
    }
    return miCatalogoLineal.datos[i];
}

// ================= COMANDO "comparar" (PR 6) =================
// Mide la misma busqueda con distintas estructuras. Cada busqueda se repite
// REPETICIONES veces porque una sola tarda menos que la resolucion del reloj.
// Ademas del tiempo se muestran los pasos (comparaciones / nodos visitados),
// que no dependen de la PC y son lo que explica la complejidad.

static const int REPETICIONES = 100000;

static double microsPorBusqueda(chrono::steady_clock::time_point a,
                                chrono::steady_clock::time_point b) {
    return chrono::duration<double, micro>(b - a).count() / REPETICIONES;
}

// Busqueda secuencial por ID en el arreglo de carga: O(n)
static int idLineal(int id, int& pasos) {
    pasos = 0;
    for (int i = 0; i < miCatalogoLineal.cantidad; i++) {
        pasos++;
        if (miCatalogoLineal.datos[i]->id == id) return i;
    }
    return -1;
}

// Conteo secuencial por rango de anios: siempre recorre las n peliculas, O(n)
static int anioLineal(int desde, int hasta, int& pasos) {
    pasos = 0;
    int total = 0;
    for (int i = 0; i < miCatalogoLineal.cantidad; i++) {
        pasos++;
        int a = miCatalogoLineal.datos[i]->anio;
        if (a >= desde && a <= hasta) total++;
    }
    return total;
}

string compararPorId(int id) {
    volatile int sumidero = 0;          // evita que el compilador quite los bucles
    int pasosLineal = 0;

    chrono::steady_clock::time_point t0 = chrono::steady_clock::now();
    for (int r = 0; r < REPETICIONES; r++) sumidero += idLineal(id, pasosLineal);
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    for (int r = 0; r < REPETICIONES; r++) sumidero += (miCatalogoHash.obtener(id) != nullptr);
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    for (int r = 0; r < REPETICIONES; r++) sumidero += miCatalogoAVL.contieneId(id);
    chrono::steady_clock::time_point t3 = chrono::steady_clock::now();
    (void)sumidero;

    const Pelicula* p = miCatalogoHash.obtener(id);
    ostringstream o;
    o << fixed << setprecision(3);
    o << "=== Comparar busqueda por ID " << id << " ("
      << (p != nullptr ? p->titulo : string("no existe")) << "), "
      << totalPeliculas() << " peliculas ===\n";
    o << "  Lineal (arreglo)   O(n)       : " << setw(8) << microsPorBusqueda(t0, t1)
      << " us | " << pasosLineal << " comparaciones\n";
    o << "  Tabla Hash         O(1) prom. : " << setw(8) << microsPorBusqueda(t1, t2)
      << " us | " << miCatalogoHash.obtenerSondeosUltimaBusqueda() << " sondeos\n";
    o << "  AVL (por anio)     O(n)       : " << setw(8) << microsPorBusqueda(t2, t3)
      << " us | " << miCatalogoAVL.obtenerNodosVisitados() << " nodos visitados\n";
    o << "  (promedio de " << REPETICIONES << " repeticiones; el AVL esta ordenado por anio,"
      << " por eso el ID se busca en el Hash)";
    return o.str();
}

string compararPorAnio(int desde, int hasta) {
    if (desde > hasta) {
        int tmp = desde;
        desde = hasta;
        hasta = tmp;
    }
    volatile int sumidero = 0;
    int pasosLineal = 0;
    int encontradas = 0;

    chrono::steady_clock::time_point t0 = chrono::steady_clock::now();
    for (int r = 0; r < REPETICIONES; r++) sumidero += anioLineal(desde, hasta, pasosLineal);
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    for (int r = 0; r < REPETICIONES; r++) sumidero += (encontradas = miCatalogoAVL.contarPorAnio(desde, hasta));
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    (void)sumidero;

    ostringstream o;
    o << fixed << setprecision(3);
    o << "=== Comparar busqueda por anio " << desde << ".." << hasta << " ("
      << encontradas << " peliculas), " << totalPeliculas() << " en total ===\n";
    o << "  Lineal (arreglo)   O(n)         : " << setw(8) << microsPorBusqueda(t0, t1)
      << " us | " << pasosLineal << " peliculas revisadas\n";
    o << "  AVL con poda       O(log n + k) : " << setw(8) << microsPorBusqueda(t1, t2)
      << " us | " << miCatalogoAVL.obtenerNodosVisitados() << " nodos visitados (altura "
      << miCatalogoAVL.obtenerAlturaRaiz() << ")\n";
    o << "  (promedio de " << REPETICIONES << " repeticiones)";
    return o.str();
}

// Registra o actualiza una calificacion en O(1) promedio
void registrarVoto(int idPelicula, int estrellasNuevas, int estrellasAnteriores) {
    miCatalogoHash.registrarVoto(idPelicula, estrellasNuevas, estrellasAnteriores);
}