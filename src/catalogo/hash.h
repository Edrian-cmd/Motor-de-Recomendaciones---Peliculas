#ifndef HASH_H
#define HASH_H

#include <string>
#include "modelo.h"

class TablaHash {
private:
    static const int TAMANIO = 1009;
    // Arreglo de punteros a Pelicula en memoria dinamica
    Pelicula* tabla[TAMANIO];
    // Suma exacta de estrellas por posicion para recalcular el promedio en O(1)
    int sumaEstrellas[TAMANIO];
    int elementosInsertados;
    int colisionesTotales;
    int sondeosUltimaBusqueda;

public:
    TablaHash();
    ~TablaHash();

    void limpiar();
    Pelicula* insertar(int id, const std::string& titulo, const std::string& genero, int anio);
    Pelicula* obtener(int id);
    std::string buscar(int id);
    void registrarVoto(int id, int estrellasNuevas, int estrellasAnteriores);

    int obtenerColisionesTotales() const;
    int obtenerSondeosUltimaBusqueda() const;
};

#endif