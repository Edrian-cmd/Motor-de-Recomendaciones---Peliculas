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

// Insercion Recursiva ordenada por ANIO
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

    // Evitamos insertar el mismo ID si coincide en el nodo actual
    if (id == nodo->id) return nodo;

    // Ordenamiento por Anio (si el anio se repite, se inserta a la derecha)
    if (anio < nodo->anio)
        nodo->izq = insertarNodo(nodo->izq, id, titulo, genero, anio);
    else
        nodo->der = insertarNodo(nodo->der, id, titulo, genero, anio);

    // 2. Actualizar la altura del nodo actual
    nodo->altura = 1 + std::max(obtenerAltura(nodo->izq), obtenerAltura(nodo->der));

    // 3. Obtener el factor de balanceo (-1, 0, 1)
    int balance = obtenerBalance(nodo);

    // 4. Aplicar rotaciones si se pierde el equilibrio
    // Caso Izquierda-Izquierda
    if (balance > 1 && obtenerBalance(nodo->izq) >= 0)
        return rotarDerecha(nodo);

    // Caso Izquierda-Derecha (Rotacion Doble)
    if (balance > 1 && obtenerBalance(nodo->izq) < 0) {
        nodo->izq = rotarIzquierda(nodo->izq);
        return rotarDerecha(nodo);
    }

    // Caso Derecha-Derecha
    if (balance < -1 && obtenerBalance(nodo->der) <= 0)
        return rotarIzquierda(nodo);

    // Caso Derecha-Izquierda (Rotacion Doble)
    if (balance < -1 && obtenerBalance(nodo->der) > 0) {
        nodo->der = rotarDerecha(nodo->der);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

void ArbolAVL::insertar(int id, std::string titulo, std::string genero, int anio) {
    raiz = insertarNodo(raiz, id, titulo, genero, anio);
}

// Recorrido recursivo In-Order para filtrar por genero (ordenado por anio)
void ArbolAVL::buscarPorGeneroNodo(NodoAVL* nodo, const std::string& genero, std::string& resultado, int& contador) {
    if (nodo == nullptr) return;

    buscarPorGeneroNodo(nodo->izq, genero, resultado, contador);

    if (nodo->genero == genero) {
        resultado += "- [" + std::to_string(nodo->anio) + "] " + nodo->titulo + " (ID: " + std::to_string(nodo->id) + ")\n";
        contador++;
    }

    buscarPorGeneroNodo(nodo->der, genero, resultado, contador);
}

std::string ArbolAVL::buscarPorGenero(const std::string& genero) {
    std::string resultado = "=== Peliculas del genero: " + genero + " ===\n";
    int contador = 0;

    buscarPorGeneroNodo(raiz, genero, resultado, contador);

    if (contador == 0) {
        return "No se encontraron peliculas para el genero: " + genero;
    }
    resultado += "Total encontradas: " + std::to_string(contador);
    return resultado;
}

// Recorrido recursivo aprovechando el ordenamiento por anio del AVL
void ArbolAVL::buscarPorAnioNodo(NodoAVL* nodo, int desde, int hasta, std::string& resultado, int& contador) {
    if (nodo == nullptr) return;

    if (nodo->anio >= desde)
        buscarPorAnioNodo(nodo->izq, desde, hasta, resultado, contador);

    if (nodo->anio >= desde && nodo->anio <= hasta) {
        resultado += "- [" + std::to_string(nodo->anio) + "] " + nodo->titulo + " (" + nodo->genero + ", ID: " + std::to_string(nodo->id) + ")\n";
        contador++;
    }

    if (nodo->anio <= hasta)
        buscarPorAnioNodo(nodo->der, desde, hasta, resultado, contador);
}

std::string ArbolAVL::buscarPorAnio(int desde, int hasta) {
    std::string resultado = "=== Peliculas entre " + std::to_string(desde) + " y " + std::to_string(hasta) + " ===\n";
    int contador = 0;

    buscarPorAnioNodo(raiz, desde, hasta, resultado, contador);

    if (contador == 0) {
        return "No se encontraron peliculas entre los anios " + std::to_string(desde) + " y " + std::to_string(hasta) + ".";
    }
    resultado += "Total encontradas: " + std::to_string(contador);
    return resultado;
}