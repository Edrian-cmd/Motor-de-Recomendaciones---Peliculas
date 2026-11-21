#ifndef GRAFO_H
#define GRAFO_H

// Integrante 4 (Oliver): El Analista Social (Simulador de Afinidad)
// Grafo no dirigido ponderado implementado con Listas de Adyacencia
// usando exclusivamente punteros y memoria dinamica (new/delete), sin STL.
//
// Diseno para el Entregable 3:
// - Los vertices son usuarios y su historial se mantiene ordenado por idPelicula.
// - La Similitud de Jaccard se calcula sobre peliculas gustadas (estrellas >= 4)
//   mediante mezcla lineal de dos punteros en O(|A| + |B|).
// - Reconstruccion perezosa (bandera sucio): evita reconstruir el grafo en cada
//   calificacion cargada desde archivo.
// - Cache dinamico de los k vecinos mas afines para responder puntajeColaborativo()
//   en O(k * |H_v|) por pelicula sin recalcular vecinos 749 veces.

#include <string>

// Umbral minimo de estrellas para considerar que una pelicula le gusto al usuario
static const int UMBRAL_GUSTO = 4;

// Nodo para la lista enlazada ordenada de peliculas calificadas por un usuario
struct NodoPelicula {
    int idPelicula;
    int estrellas;          // Escala de 1 a 5 estrellas
    NodoPelicula* sig;
};

struct VerticeUsuario;

// Nodo para la lista de adyacencia (Arista Usuario -> Usuario con similitud > 0)
struct Arista {
    int idDestino;
    VerticeUsuario* destino;    // Puntero directo al vertice destino en O(1)
    double similitud;           // Peso de Jaccard entre 0.0 y 1.0
    int interseccion;           // |A n B| sobre peliculas gustadas (>= 4)
    int unionTotal;             // |A u B| sobre peliculas gustadas (>= 4)
    Arista* sig;
};

// Nodo principal del Grafo (Vertice = Usuario)
struct VerticeUsuario {
    int id;
    std::string nombre;
    NodoPelicula* historial;    // Lista ordenada ascendente por idPelicula
    Arista* adyacentes;         // Lista de conexiones con similitud > 0
    VerticeUsuario* sig;
};

// Entrada del cache de los k vecinos mas afines al usuario consultado
struct VecinoCache {
    VerticeUsuario* usuario;
    double similitud;
};

class GrafoAfinidad {
public:
    GrafoAfinidad();
    ~GrafoAfinidad();

    // Inserta un vertice usuario si no existe. Complejidad: O(V)
    bool insertarUsuario(int id, const std::string& nombre);

    // Agrega o actualiza una pelicula manteniendo orden por idPelicula.
    // Si estrellas == 0, elimina la pelicula del historial. Complejidad: O(|H|)
    bool agregarPelicula(int idUsuario, int idPelicula, int estrellas);

    // Elimina una pelicula del historial del usuario (usado por deshacer).
    // Complejidad: O(|H|)
    bool eliminarPelicula(int idUsuario, int idPelicula);

    // Reconstruye todas las aristas con Jaccard > 0.
    // Complejidad: O(V^2 * (|H_a| + |H_b|))
    void construirRedAfinidad();

    // Reconstruye el grafo solo si hubo cambios desde la ultima consulta.
    void asegurarActualizado();
    bool estaSucio() const;

    // Consultas sobre vertices, aristas e historial
    VerticeUsuario* buscarVertice(int id) const;                    // O(V)
    Arista* buscarArista(int idOrigen, int idDestino) const;        // O(V + grado)
    int obtenerEstrellas(int idUsuario, int idPelicula) const;      // O(V + |H|)
    bool usuarioVioPelicula(int idUsuario, int idPelicula) const;   // O(V + |H|)

    // Similitud de Jaccard sobre peliculas gustadas (>= 4) por mezcla lineal.
    // Complejidad: O(V + |A| + |B|)
    double calcularJaccard(int idA, int idB, int& tamInterseccion, int& tamUnion) const;

    // Puntaje colaborativo en [0, 1] para idPelicula usando los k vecinos mas afines
    // al usuario indicado (con cache interno). Complejidad amortizada: O(k * |H_v|)
    double calcularPuntajeColaborativo(int idUsuario, int idPelicula, int k = 10);

    // Metodos de diagnostico para el Modo Desarrollador (Funcionalidad 6)
    int totalVertices() const;
    int totalAristas() const;
    int pasosUltimoRecorrido() const;
    int tamanoCacheVecinos() const;
    std::string estadoAdyacencia() const;
    std::string estadoAdyacenciaUsuario(int idUsuario) const;

private:
    VerticeUsuario* cabeza;
    int numVertices;
    int numAristas;
    bool sucio;
    mutable int pasosRecorrido;

    // Cache dinamico para puntajeColaborativo (sin STL)
    VecinoCache* cacheVecinos;
    int tamCache;
    int capacidadCache;
    int usuarioCache;
    int kCache;

    void marcarSucio();
    void limpiarAristas();
    void insertarAristaDirigida(VerticeUsuario* origen, VerticeUsuario* destino,
                                double similitud, int interseccion, int unionTotal);

    double calcularJaccardVertices(const VerticeUsuario* uA, const VerticeUsuario* uB,
                                   int& tamInterseccion, int& tamUnion) const;
    int buscarEstrellasEnLista(const NodoPelicula* historial, int idPelicula) const;
    void actualizarCacheVecinos(int idUsuario, int k);

    static const NodoPelicula* siguienteGustada(const NodoPelicula* nodo);
    static bool mejorVecino(double simA, int idA, double simB, int idB);

    // No copiable: el grafo es dueno de toda su memoria dinamica
    GrafoAfinidad(const GrafoAfinidad&);
    GrafoAfinidad& operator=(const GrafoAfinidad&);
};

#endif