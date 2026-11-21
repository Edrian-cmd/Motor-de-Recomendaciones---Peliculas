#include "avl.h"
#include <algorithm>
#include <cctype>

// Comparacion de generos sin distinguir mayusculas: "drama" == "Drama". O(len)
static bool mismoGenero(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++)
        if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i]))
            return false;
    return true;
}

ArbolAVL::ArbolAVL() {
    raiz = nullptr;
    rotacionesTotales = 0;
    nodosVisitadosUltimaBusqueda = 0;
}

ArbolAVL::~ArbolAVL() {
    limpiar();
}

// Recorrido post-orden para liberar memoria dinamica: O(n)
void ArbolAVL::destruirNodo(NodoAVL* nodo) {
    if (nodo == nullptr) return;
    destruirNodo(nodo->izq);
    destruirNodo(nodo->der);
    delete nodo;
}

void ArbolAVL::limpiar() {
    destruirNodo(raiz);
    raiz = nullptr;
    rotacionesTotales = 0;
    nodosVisitadosUltimaBusqueda = 0;
}

int ArbolAVL::obtenerAltura(NodoAVL* nodo) const {
    if (nodo == nullptr) return 0;
    return nodo->altura;
}

int ArbolAVL::obtenerBalance(NodoAVL* nodo) const {
    if (nodo == nullptr) return 0;
    return obtenerAltura(nodo->izq) - obtenerAltura(nodo->der);
}

// Rotacion Simple a la Derecha: O(1)
NodoAVL* ArbolAVL::rotarDerecha(NodoAVL* y) {
    NodoAVL* x = y->izq;
    NodoAVL* T2 = x->der;

    x->der = y;
    y->izq = T2;

    y->altura = std::max(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    x->altura = std::max(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    rotacionesTotales++;
    return x;
}

// Rotacion Simple a la Izquierda: O(1)
NodoAVL* ArbolAVL::rotarIzquierda(NodoAVL* x) {
    NodoAVL* y = x->der;
    NodoAVL* T2 = y->izq;

    y->izq = x;
    x->der = T2;

    x->altura = std::max(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
    y->altura = std::max(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
    rotacionesTotales++;
    return y;
}

// Insercion recursiva balanceada ordenada por ANIO: O(log n)
NodoAVL* ArbolAVL::insertarNodo(NodoAVL* nodo, int id, const std::string& titulo, const std::string& genero, int anio) {
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

    if (id == nodo->id) return nodo;

    if (anio < nodo->anio)
        nodo->izq = insertarNodo(nodo->izq, id, titulo, genero, anio);
    else
        nodo->der = insertarNodo(nodo->der, id, titulo, genero, anio);

    nodo->altura = 1 + std::max(obtenerAltura(nodo->izq), obtenerAltura(nodo->der));
    int balance = obtenerBalance(nodo);

    // Caso Izquierda-Izquierda
    if (balance > 1 && obtenerBalance(nodo->izq) >= 0)
        return rotarDerecha(nodo);

    // Caso Izquierda-Derecha
    if (balance > 1 && obtenerBalance(nodo->izq) < 0) {
        nodo->izq = rotarIzquierda(nodo->izq);
        return rotarDerecha(nodo);
    }

    // Caso Derecha-Derecha
    if (balance < -1 && obtenerBalance(nodo->der) <= 0)
        return rotarIzquierda(nodo);

    // Caso Derecha-Izquierda
    if (balance < -1 && obtenerBalance(nodo->der) > 0) {
        nodo->der = rotarDerecha(nodo->der);
        return rotarIzquierda(nodo);
    }

    return nodo;
}

void ArbolAVL::insertar(int id, const std::string& titulo, const std::string& genero, int anio) {
    raiz = insertarNodo(raiz, id, titulo, genero, anio);
}

// Recorrido In-Order para filtrar por genero ordenado por anio: O(n)
void ArbolAVL::buscarPorGeneroNodo(NodoAVL* nodo, const std::string& genero, std::string& resultado, int& contador) {
    if (nodo == nullptr) return;

    nodosVisitadosUltimaBusqueda++;
    buscarPorGeneroNodo(nodo->izq, genero, resultado, contador);

    if (mismoGenero(nodo->genero, genero)) {
        resultado += "- [" + std::to_string(nodo->anio) + "] " + nodo->titulo + " (ID: " + std::to_string(nodo->id) + ")\n";
        contador++;
    }

    buscarPorGeneroNodo(nodo->der, genero, resultado, contador);
}

std::string ArbolAVL::buscarPorGenero(const std::string& genero) {
    std::string resultado = "=== Peliculas del genero: " + genero + " ===\n";
    int contador = 0;
    nodosVisitadosUltimaBusqueda = 0;

    buscarPorGeneroNodo(raiz, genero, resultado, contador);

    if (contador == 0) {
        return "No se encontraron peliculas para el genero: " + genero;
    }
    resultado += "Total encontradas: " + std::to_string(contador);
    return resultado;
}

// Busqueda por rango de anios con poda de subarboles: O(log n + k)
void ArbolAVL::buscarPorAnioNodo(NodoAVL* nodo, int desde, int hasta, std::string& resultado, int& contador) {
    if (nodo == nullptr) return;

    nodosVisitadosUltimaBusqueda++;

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
    nodosVisitadosUltimaBusqueda = 0;

    buscarPorAnioNodo(raiz, desde, hasta, resultado, contador);

    if (contador == 0) {
        return "No se encontraron peliculas entre los anios " + std::to_string(desde) + " y " + std::to_string(hasta) + ".";
    }
    resultado += "Total encontradas: " + std::to_string(contador);
    return resultado;
}

// Busqueda por ID en un arbol ordenado por ANIO: no se puede podar, hay que
// recorrerlo (pre-orden, se detiene al encontrarlo). O(n). Sirve en "comparar"
// para mostrar por que el ID se busca en la Tabla Hash y no en el AVL.
bool ArbolAVL::contieneIdNodo(NodoAVL* nodo, int id) {
    if (nodo == nullptr) return false;
    nodosVisitadosUltimaBusqueda++;
    if (nodo->id == id) return true;
    return contieneIdNodo(nodo->izq, id) || contieneIdNodo(nodo->der, id);
}

bool ArbolAVL::contieneId(int id) {
    nodosVisitadosUltimaBusqueda = 0;
    return contieneIdNodo(raiz, id);
}

// Igual que buscarPorAnioNodo (misma poda) pero solo cuenta: O(log n + k)
int ArbolAVL::contarPorAnioNodo(NodoAVL* nodo, int desde, int hasta) {
    if (nodo == nullptr) return 0;
    nodosVisitadosUltimaBusqueda++;
    int total = 0;
    if (nodo->anio >= desde)
        total += contarPorAnioNodo(nodo->izq, desde, hasta);
    if (nodo->anio >= desde && nodo->anio <= hasta)
        total++;
    if (nodo->anio <= hasta)
        total += contarPorAnioNodo(nodo->der, desde, hasta);
    return total;
}

int ArbolAVL::contarPorAnio(int desde, int hasta) {
    nodosVisitadosUltimaBusqueda = 0;
    return contarPorAnioNodo(raiz, desde, hasta);
}

int ArbolAVL::obtenerAlturaRaiz() const {
    return obtenerAltura(raiz);
}

int ArbolAVL::obtenerRotacionesTotales() const {
    return rotacionesTotales;
}

int ArbolAVL::obtenerNodosVisitados() const {
    return nodosVisitadosUltimaBusqueda;
}