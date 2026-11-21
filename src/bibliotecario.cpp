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

// Registra o actualiza una calificacion en O(1) promedio
void registrarVoto(int idPelicula, int estrellasNuevas, int estrellasAnteriores) {
    miCatalogoHash.registrarVoto(idPelicula, estrellasNuevas, estrellasAnteriores);
}