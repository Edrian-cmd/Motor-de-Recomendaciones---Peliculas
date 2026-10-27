#include "avl.h"

ArbolAVL::ArbolAVL() {
    raiz = nullptr;
}

void ArbolAVL::insertar(int id, std::string titulo, std::string genero, int anio) {
    // Logica de insercion recursiva y balanceo (rotaciones) pendiente
}

std::string ArbolAVL::buscarPorRango(int desde, int hasta) {
    // STUB: Simulacion para el Entregable 2
    return "Resultado desde Arbol AVL: Filtrando peliculas entre " + std::to_string(desde) + " y " + std::to_string(hasta);
}

std::string ArbolAVL::buscarPorGenero(const std::string& genero) {
    // STUB: Simulacion para el Entregable 2
    return "Resultado desde Arbol AVL: Filtrando genero " + genero;
}