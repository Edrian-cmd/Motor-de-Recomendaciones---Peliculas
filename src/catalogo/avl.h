#ifndef AVL_H
#define AVL_H

#include <string>

struct NodoAVL {
    int id;
    std::string titulo;
    std::string genero;
    int anio;
    int altura;
    NodoAVL* izq;
    NodoAVL* der;
};

class ArbolAVL {
private:
    NodoAVL* raiz;
    int rotacionesTotales;
    int nodosVisitadosUltimaBusqueda;

    int obtenerAltura(NodoAVL* nodo) const;
    int obtenerBalance(NodoAVL* nodo) const;
    NodoAVL* rotarDerecha(NodoAVL* y);
    NodoAVL* rotarIzquierda(NodoAVL* x);
    NodoAVL* insertarNodo(NodoAVL* nodo, int id, const std::string& titulo, const std::string& genero, int anio);
    void buscarPorGeneroNodo(NodoAVL* nodo, const std::string& genero, std::string& resultado, int& contador);
    void buscarPorAnioNodo(NodoAVL* nodo, int desde, int hasta, std::string& resultado, int& contador);
    void destruirNodo(NodoAVL* nodo);
    bool contieneIdNodo(NodoAVL* nodo, int id);
    int contarPorAnioNodo(NodoAVL* nodo, int desde, int hasta);

public:
    ArbolAVL();
    ~ArbolAVL();

    void limpiar();
    void insertar(int id, const std::string& titulo, const std::string& genero, int anio);
    std::string buscarPorGenero(const std::string& genero);
    std::string buscarPorAnio(int desde, int hasta);

    // Para el comando "comparar" (solo cuentan, no arman texto):
    bool contieneId(int id);                   // O(n): el arbol esta ordenado por anio, no por id
    int contarPorAnio(int desde, int hasta);   // O(log n + k) con poda

    int obtenerAlturaRaiz() const;
    int obtenerRotacionesTotales() const;
    int obtenerNodosVisitados() const;
};

#endif