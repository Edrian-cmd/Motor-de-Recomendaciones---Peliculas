#include "hash.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

TablaHash::TablaHash() {
    elementosInsertados = 0;
    colisionesTotales = 0;
    sondeosUltimaBusqueda = 0;
    for (int i = 0; i < TAMANIO; i++) {
        tabla[i] = nullptr;
        sumaEstrellas[i] = 0;
    }
}

TablaHash::~TablaHash() {
    limpiar();
}

// Libera toda la memoria dinamica creada con new: O(m) donde m = 1009
void TablaHash::limpiar() {
    for (int i = 0; i < TAMANIO; i++) {
        if (tabla[i] != nullptr) {
            delete tabla[i];
            tabla[i] = nullptr;
        }
        sumaEstrellas[i] = 0;
    }
    elementosInsertados = 0;
    colisionesTotales = 0;
    sondeosUltimaBusqueda = 0;
}

// Insercion con exploracion lineal (Linear Probing): O(1) promedio, O(m) peor caso
Pelicula* TablaHash::insertar(int id, const std::string& titulo, const std::string& genero, int anio) {
    int indice = std::abs(id) % TAMANIO;
    int inicio = indice;

    while (tabla[indice] != nullptr) {
        // Si el ID ya existe, devolvemos nullptr para no duplicar en el arreglo lineal
        if (tabla[indice]->id == id) return nullptr;

        colisionesTotales++;
        indice = (indice + 1) % TAMANIO;

        if (indice == inicio) {
            std::cout << "[ADVERTENCIA] Capacidad maxima de la Tabla Hash (1009) alcanzada. No se pudo insertar: " << titulo << std::endl;
            return nullptr;
        }
    }

    Pelicula* nuevaPelicula = new Pelicula;
    nuevaPelicula->id = id;
    nuevaPelicula->titulo = titulo;
    nuevaPelicula->genero = genero;
    nuevaPelicula->anio = anio;
    nuevaPelicula->ratingPromedio = 0.0;
    nuevaPelicula->numVotos = 0;

    tabla[indice] = nuevaPelicula;
    sumaEstrellas[indice] = 0;
    elementosInsertados++;
    return nuevaPelicula;
}

// Obtiene el puntero a la Pelicula por ID: O(1) promedio
Pelicula* TablaHash::obtener(int id) {
    int indice = std::abs(id) % TAMANIO;
    int inicio = indice;
    sondeosUltimaBusqueda = 1;

    while (tabla[indice] != nullptr) {
        if (tabla[indice]->id == id) {
            return tabla[indice];
        }
        indice = (indice + 1) % TAMANIO;
        sondeosUltimaBusqueda++;
        if (indice == inicio) break;
    }

    return nullptr;
}

// Busqueda formateada en texto por ID: O(1) promedio
std::string TablaHash::buscar(int id) {
    Pelicula* p = obtener(id);
    if (p != nullptr) {
        std::ostringstream oss;
        oss << "ID " << id << " encontrado: " << p->titulo
            << " (" << p->genero << ", " << p->anio << ") | Rating: "
            << std::fixed << std::setprecision(2) << p->ratingPromedio
            << " (" << p->numVotos << " votos)";
        return oss.str();
    }

    return "Fallo O(1): La pelicula con ID " + std::to_string(id) + " no existe en el catalogo.";
}

// Actualiza ratingPromedio y numVotos en O(1) promedio segun la convencion de 0 estrellas
void TablaHash::registrarVoto(int id, int estrellasNuevas, int estrellasAnteriores) {
    int indice = std::abs(id) % TAMANIO;
    int inicio = indice;

    while (tabla[indice] != nullptr) {
        if (tabla[indice]->id == id) {
            sumaEstrellas[indice] += (estrellasNuevas - estrellasAnteriores);
            if (sumaEstrellas[indice] < 0) {
                sumaEstrellas[indice] = 0;
            }

            int deltaVotos = (estrellasNuevas > 0 ? 1 : 0) - (estrellasAnteriores > 0 ? 1 : 0);
            tabla[indice]->numVotos += deltaVotos;
            if (tabla[indice]->numVotos < 0) {
                tabla[indice]->numVotos = 0;
            }

            if (tabla[indice]->numVotos > 0) {
                tabla[indice]->ratingPromedio = static_cast<double>(sumaEstrellas[indice]) / tabla[indice]->numVotos;
            } else {
                tabla[indice]->ratingPromedio = 0.0;
                sumaEstrellas[indice] = 0;
            }
            return;
        }
        indice = (indice + 1) % TAMANIO;
        if (indice == inicio) break;
    }
}

int TablaHash::obtenerColisionesTotales() const {
    return colisionesTotales;
}

int TablaHash::obtenerSondeosUltimaBusqueda() const {
    return sondeosUltimaBusqueda;
}