#include "hash.h"

TablaHash::TablaHash() {
    for(int i = 0; i < 1009; i++) {
        tabla[i] = nullptr;
    }
}

void TablaHash::insertar(int id, std::string titulo, std::string genero, int anio) {
    // Funcion Hash: Modulo del tamaño del arreglo para calcular la posicion
    int indice = id % 1009;
    int inicio = indice;

    // Resolucion de Colisiones: Exploracion Lineal (Linear Probing)
    while (tabla[indice] != nullptr) {
        // Si el ID ya existe en la memoria, evitamos duplicados
        if (tabla[indice]->id == id) return; 
        
        // Si el espacio esta ocupado, avanzamos al siguiente indice disponible
        indice = (indice + 1) % 1009;
        
        // Si dimos la vuelta completa, el arreglo esta lleno
        if (indice == inicio) return; 
    }

    // Creacion del Nodo usando Memoria Dinamica
    Pelicula* nuevaPelicula = new Pelicula;
    nuevaPelicula->id = id;
    nuevaPelicula->titulo = titulo;
    nuevaPelicula->genero = genero;
    nuevaPelicula->anio = anio;

    // Guardamos el puntero en la posicion calculada
    tabla[indice] = nuevaPelicula;
}

std::string TablaHash::buscar(int id) {
    int indice = id % 1009;
    int inicio = indice;

    // Buscamos la pelicula siguiendo la misma ruta de colisiones
    while (tabla[indice] != nullptr) {
        if (tabla[indice]->id == id) {
            return "ID " + std::to_string(id) + " encontrado: " + tabla[indice]->titulo + " (" + tabla[indice]->genero + ")";
        }
        indice = (indice + 1) % 1009;
        if (indice == inicio) break; 
    }

    return "Fallo O(1): La pelicula con ID " + std::to_string(id) + " no existe en el catalogo.";
}