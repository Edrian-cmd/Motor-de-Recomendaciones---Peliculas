#include "avl.h"
#include <algorithm> // Para std::max

ArbolAVL::ArbolAVL() {
    raiz = nullptr;
}

int ArbolAVL::obtenerAltura(NodoAVL* nodo) {
    if (nodo == nullptr) return 0;
    return nodo->altura;
}

int ArbolAVL::obtenerBalance(NodoAVL* nodo) {
    if (nodo == nullptr) return 0;
    return obtenerAltura(nodo->izq) - obtenerAltura(nodo->der);
}

// Rotacion Simple a la Derecha
NodoAVL* ArbolAVL::rotarDerecha(NodoAVL* y) {
    NodoAVL* x = y->izq;
    NodoAVL* T2 = x->der;

    x->der = y;
    y->izq = T2;

    y->altura = std::max(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    x->altura = std::max(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    return x;
}

// Rotacion Simple a la Izquierda
NodoAVL* ArbolAVL::rotarIzquierda(NodoAVL* x) {
    NodoAVL* y = x->der;
    NodoAVL* T2 = y->izq;

    y->izq = x;
    x->der = T2;

    x->altura = std::max(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    y->altura = std::max(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    return y;
}

// Insercion Recursiva
NodoAVL* ArbolAVL::insertarNodo(NodoAVL* nodo, int id, std::string titulo, std::string genero, int anio) {
    // 1. Insercion estandar de Arbol Binario
    if (nodo == nullptr) {
        NodoAVL* nuevoNodo = new NodoAVL;
        nuevoNodo->id = id;
        nuevoNodo->titulo = titulo;
        nuevoNodo->genero = genero;
        nuevoNodo->anio = anio;
        nuevoNodo->altura = 1;
        nuevoNodo->izq = nullptr;
        nuevoNodo->der = nullptr;
        return nuevoNodo;
    }

    if (id < nodo->id)
        nodo->izq = insertarNodo(nodo->izq, id, titulo, genero, anio);
    else if (id > nodo->id)
        nodo->der = insertarNodo(nodo->der, id, titulo, genero, anio);
    else
        return nodo; // Ignoramos peliculas duplicadas

    // 2. Actualizar la altura del nodo actual
    nodo->altura = 1 + std::max(obtenerAltura(nodo->izq), obtenerAltura(nodo->der));

    // 3. Obtener el factor de balanceo (-1, 0, 1)
    int balance = obtenerBalance(nodo);

    // 4. Aplicar rotaciones si se pierde el equilibrio
    // Caso Izquierda-Izquierda
    if (balance > 1 && id < nodo->izq->id)
        return rotarDerecha(nodo);

    // Caso Derecha-Derecha
    if (balance < -1 && id > nodo->der->id)
        return rotarIzquierda(nodo);

    // Caso Izquierda-Derecha (Rotacion Doble)
    if (balance > 1 && id > nodo->izq->id) {
        nodo->izq = rotarIzquierda(nodo->izq);
        return rotarDerecha(nodo);
    }

    // Caso Derecha-Izquierda (Rotacion Doble)
    if (balance < -1 && id < nodo->der->id) {
        nodo->der = rotarDerecha(nodo->der);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

void ArbolAVL::insertar(int id, std::string titulo, std::string genero, int anio) {
    raiz = insertarNodo(raiz, id, titulo, genero, anio);
}