#include "grafo.h"
#include <sstream>
#include <iomanip>

using namespace std;

GrafoAfinidad::GrafoAfinidad()
    : cabeza(nullptr), numVertices(0), numAristas(0), pasosRecorrido(0) {}

GrafoAfinidad::~GrafoAfinidad() {
    limpiarAristas();
    VerticeUsuario* actual = cabeza;
    while (actual != nullptr) {
        // 1. Liberar memoria de la lista de peliculas del usuario
        NodoPelicula* pel = actual->historial;
        while (pel != nullptr) {
            NodoPelicula* borrarPel = pel;
            pel = pel->sig;
            delete borrarPel;
        }
        // 2. Liberar el vertice del usuario
        VerticeUsuario* borrarVert = actual;
        actual = actual->sig;
        delete borrarVert;
    }
}

void GrafoAfinidad::limpiarAristas() {
    VerticeUsuario* v = cabeza;
    while (v != nullptr) {
        Arista* a = v->adyacentes;
        while (a != nullptr) {
            Arista* borrarArista = a;
            a = a->sig;
            delete borrarArista;
        }
        v->adyacentes = nullptr;
        v = v->sig;
    }
    numAristas = 0;
}

VerticeUsuario* GrafoAfinidad::buscarVertice(int id) const {
    VerticeUsuario* actual = cabeza;
    while (actual != nullptr) {
        pasosRecorrido++;
        if (actual->id == id)
            return actual;
        actual = actual->sig;
    }
    return nullptr;
}

bool GrafoAfinidad::insertarUsuario(int id, const string& nombre) {
    if (buscarVertice(id) != nullptr)
        return false; // El usuario ya existe

    VerticeUsuario* nuevo = new VerticeUsuario;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->historial = nullptr;
    nuevo->adyacentes = nullptr;
    nuevo->sig = nullptr;

    // Insertar al final para mantener el orden de los IDs
    if (cabeza == nullptr) {
        cabeza = nuevo;
    } else {
        VerticeUsuario* temp = cabeza;
        while (temp->sig != nullptr)
            temp = temp->sig;
        temp->sig = nuevo;
    }

    numVertices++;
    return true;
}

bool GrafoAfinidad::usuarioVioPelicula(int idUsuario, int idPelicula) const {
    VerticeUsuario* u = buscarVertice(idUsuario);
    if (u == nullptr) return false;

    NodoPelicula* actual = u->historial;
    while (actual != nullptr) {
        pasosRecorrido++;
        if (actual->idPelicula == idPelicula)
            return true;
        actual = actual->sig;
    }
    return false;
}

bool GrafoAfinidad::agregarPelicula(int idUsuario, int idPelicula, int estrellas) {
    VerticeUsuario* u = buscarVertice(idUsuario);
    if (u == nullptr) return false;

    // Si ya existe la pelicula en su historial, actualizamos las estrellas
    NodoPelicula* actual = u->historial;
    while (actual != nullptr) {
        if (actual->idPelicula == idPelicula) {
            actual->estrellas = estrellas;
            return true;
        }
        actual = actual->sig;
    }

    // Crear nuevo nodo de pelicula en memoria dinamica
    NodoPelicula* nueva = new NodoPelicula;
    nueva->idPelicula = idPelicula;
    nueva->estrellas = estrellas;
    nueva->sig = u->historial;
    u->historial = nueva;
    return true;
}

double GrafoAfinidad::calcularJaccard(int idA, int idB, int& tamInterseccion, int& tamUnion) const {
    tamInterseccion = 0;
    tamUnion = 0;

    VerticeUsuario* uA = buscarVertice(idA);
    VerticeUsuario* uB = buscarVertice(idB);
    if (uA == nullptr || uB == nullptr)
        return 0.0;

    // Contar peliculas de A y cuantas coinciden en B (Interseccion)
    int totalA = 0;
    NodoPelicula* pA = uA->historial;
    while (pA != nullptr) {
        totalA++;
        NodoPelicula* pB = uB->historial;
        while (pB != nullptr) {
            pasosRecorrido++;
            if (pA->idPelicula == pB->idPelicula) {
                tamInterseccion++;
                break;
            }
            pB = pB->sig;
        }
        pA = pA->sig;
    }

    // Contar peliculas de B
    int totalB = 0;
    NodoPelicula* pB = uB->historial;
    while (pB != nullptr) {
        totalB++;
        pB = pB->sig;
    }

    // Propiedad de conjuntos: |A u B| = |A| + |B| - |A n B|
    tamUnion = totalA + totalB - tamInterseccion;
    if (tamUnion == 0)
        return 0.0;

    return (double)tamInterseccion / (double)tamUnion;
}

void GrafoAfinidad::insertarAristaDirigida(VerticeUsuario* origen, int idDestino,
                                           double similitud, int interseccion, int unionTotal) {
    Arista* nueva = new Arista;
    nueva->idDestino = idDestino;
    nueva->similitud = similitud;
    nueva->interseccion = interseccion;
    nueva->unionTotal = unionTotal;
    nueva->sig = origen->adyacentes;
    origen->adyacentes = nueva;
}

void GrafoAfinidad::construirRedAfinidad() {
    limpiarAristas();
    pasosRecorrido = 0;

    VerticeUsuario* uA = cabeza;
    while (uA != nullptr) {
        VerticeUsuario* uB = uA->sig;
        while (uB != nullptr) {
            int inter = 0, uni = 0;
            double jaccard = calcularJaccard(uA->id, uB->id, inter, uni);

            // Conectamos ambos vertices (Grafo No Dirigido)
            insertarAristaDirigida(uA, uB->id, jaccard, inter, uni);
            insertarAristaDirigida(uB, uA->id, jaccard, inter, uni);
            numAristas++;

            uB = uB->sig;
        }
        uA = uA->sig;
    }
}

Arista* GrafoAfinidad::buscarArista(int idOrigen, int idDestino) const {
    pasosRecorrido = 0;
    VerticeUsuario* origen = buscarVertice(idOrigen);
    if (origen == nullptr) return nullptr;

    Arista* a = origen->adyacentes;
    while (a != nullptr) {
        pasosRecorrido++;
        if (a->idDestino == idDestino)
            return a;
        a = a->sig;
    }
    return nullptr;
}

int GrafoAfinidad::totalVertices() const {
    return numVertices;
}

int GrafoAfinidad::totalAristas() const {
    return numAristas;
}

int GrafoAfinidad::pasosUltimoRecorrido() const {
    return pasosRecorrido;
}

string GrafoAfinidad::estadoAdyacencia() const {
    ostringstream o;
    o << fixed << setprecision(2);
    VerticeUsuario* v = cabeza;
    while (v != nullptr) {
        o << "U" << v->id << "->[";
        Arista* a = v->adyacentes;
        bool primero = true;
        while (a != nullptr) {
            if (!primero) o << ", ";
            o << "U" << a->idDestino << ":" << a->similitud;
            primero = false;
            a = a->sig;
        }
        o << "]";
        if (v->sig != nullptr) o << " | ";
        v = v->sig;
    }
    return o.str();
}