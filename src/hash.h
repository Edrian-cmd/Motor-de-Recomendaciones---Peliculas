#ifndef HASH_H
#define HASH_H
#include <string>

struct Pelicula {
    int id;
    std::string titulo;
    std::string genero;
    int anio;
};

class TablaHash {
private:
    // Arreglo de punteros para manejar la memoria dinamica
    Pelicula* tabla[100]; 
public:
    TablaHash();
    void insertar(int id, std::string titulo, std::string genero, int anio);
    std::string buscar(int id);
};

#endif