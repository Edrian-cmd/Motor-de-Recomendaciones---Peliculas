#ifndef AVL_H
#define AVL_H
#include <string>

struct NodoAVL {
    int id;
    std::string titulo;
    std::string genero;
    int anio;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;
};

class ArbolAVL {
private:
    NodoAVL* raiz;
    // Las funciones de rotacion (Izquierda, Derecha, Dobles) se programaran despues
public:
    ArbolAVL();
    void insertar(int id, std::string titulo, std::string genero, int anio);
    std::string buscarPorRango(int desde, int hasta);
    std::string buscarPorGenero(const std::string& genero);
};

#endif