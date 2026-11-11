#ifndef GRAFO_H
#define GRAFO_H


#include <string>

// Nodo para la lista enlazada de peliculas calificadas por un usuario
struct NodoPelicula {
    int idPelicula;
    int estrellas;          // Escala de 1 a 5 estrellas
    NodoPelicula* sig;
};

// Nodo para la lista de adyacencia (Arista Usuario -> Usuario)
struct Arista {
    int idDestino;
    double similitud;       // Peso de Jaccard entre 0.0 y 1.0
    int interseccion;       // |A n B|: Peliculas vistas por ambos
    int unionTotal;         // |A u B|: Total de peliculas unicas entre ambos
    Arista* sig;
};

// Nodo principal del Grafo (Vertice = Usuario)
struct VerticeUsuario {
    int id;
    std::string nombre;
    NodoPelicula* historial;    // Puntero a sus peliculas vistas
    Arista* adyacentes;         // Puntero a sus conexiones con otros usuarios
    VerticeUsuario* sig;
};

class GrafoAfinidad {
public:
    GrafoAfinidad();
    ~GrafoAfinidad();

    // Operaciones de construccion del grafo
    bool insertarUsuario(int id, const std::string& nombre);
    bool agregarPelicula(int idUsuario, int idPelicula, int estrellas);
    
    // Calcula la Similitud de Jaccard entre todos los usuarios y crea las aristas
    void construirRedAfinidad();

    // Consultas sobre el grafo
    VerticeUsuario* buscarVertice(int id) const;
    Arista* buscarArista(int idOrigen, int idDestino) const;
    bool usuarioVioPelicula(int idUsuario, int idPelicula) const;

    // Calculo matematico directo de Jaccard entre dos usuarios
    double calcularJaccard(int idA, int idB, int& tamInterseccion, int& tamUnion) const;

    // Metodos para el Modo Desarrollador (Funcionalidad 6)
    int totalVertices() const;
    int totalAristas() const;
    int pasosUltimoRecorrido() const;
    std::string estadoAdyacencia() const;

private:
    VerticeUsuario* cabeza;
    int numVertices;
    int numAristas;
    mutable int pasosRecorrido;

    void limpiarAristas();
    void insertarAristaDirigida(VerticeUsuario* origen, int idDestino,
                                double similitud, int interseccion, int unionTotal);

    // No copiable: el grafo administra su propia memoria dinamica
    GrafoAfinidad(const GrafoAfinidad&);
    GrafoAfinidad& operator=(const GrafoAfinidad&);
};

#endif