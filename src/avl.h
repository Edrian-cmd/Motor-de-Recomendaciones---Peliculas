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
    int obtenerAltura(NodoAVL* nodo);
    int obtenerBalance(NodoAVL* nodo);
    NodoAVL* rotarDerecha(NodoAVL* y);
    NodoAVL* rotarIzquierda(NodoAVL* x);
    NodoAVL* insertarNodo(NodoAVL* nodo, int id, std::string titulo, std::string genero, int anio);
    
public:
    ArbolAVL();
    void insertar(int id, std::string titulo, std::string genero, int anio);
};
#endif