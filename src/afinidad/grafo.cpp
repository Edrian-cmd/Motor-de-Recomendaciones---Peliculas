#include "grafo.h"
#include <sstream>
#include <iomanip>

using namespace std;

GrafoAfinidad::GrafoAfinidad()
    : cabeza(nullptr),
      numVertices(0),
      numAristas(0),
      sucio(false),
      pasosRecorrido(0),
      cacheVecinos(nullptr),
      tamCache(0),
      capacidadCache(0),
      usuarioCache(-1),
      kCache(0) {}

// Libera cada arista, el cache de vecinos, cada nodo de pelicula y cada vertice.
// Complejidad: O(V + E + totalCalificaciones)
GrafoAfinidad::~GrafoAfinidad() {
    limpiarAristas();
    delete[] cacheVecinos;
    cacheVecinos = nullptr;

    VerticeUsuario* actual = cabeza;
    while (actual != nullptr) {
        NodoPelicula* pel = actual->historial;
        while (pel != nullptr) {
            NodoPelicula* borrarPel = pel;
            pel = pel->sig;
            delete borrarPel;
        }
        VerticeUsuario* borrarVert = actual;
        actual = actual->sig;
        delete borrarVert;
    }
}

void GrafoAfinidad::marcarSucio() {
    sucio = true;
    usuarioCache = -1;
    tamCache = 0;
}

bool GrafoAfinidad::estaSucio() const {
    return sucio;
}

void GrafoAfinidad::asegurarActualizado() {
    if (sucio) {
        construirRedAfinidad();
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
    usuarioCache = -1;
    tamCache = 0;
}

// Busqueda lineal del vertice usuario por ID. Complejidad: O(V)
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

// Inserta un nuevo usuario al final para conservar orden de creacion.
// Complejidad: O(V)
bool GrafoAfinidad::insertarUsuario(int id, const string& nombre) {
    if (buscarVertice(id) != nullptr)
        return false;

    VerticeUsuario* nuevo = new VerticeUsuario;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->historial = nullptr;
    nuevo->adyacentes = nullptr;
    nuevo->sig = nullptr;

    if (cabeza == nullptr) {
        cabeza = nuevo;
    } else {
        VerticeUsuario* temp = cabeza;
        while (temp->sig != nullptr)
            temp = temp->sig;
        temp->sig = nuevo;
    }

    numVertices++;
    marcarSucio();
    return true;
}

// Busca las estrellas de una pelicula en una lista ordenada ascendente.
// Se detiene temprano apenas supera idPelicula. Complejidad: O(|H|)
int GrafoAfinidad::buscarEstrellasEnLista(const NodoPelicula* historial, int idPelicula) const {
    const NodoPelicula* actual = historial;
    while (actual != nullptr && actual->idPelicula <= idPelicula) {
        pasosRecorrido++;
        if (actual->idPelicula == idPelicula)
            return actual->estrellas;
        actual = actual->sig;
    }
    return 0;
}

int GrafoAfinidad::obtenerEstrellas(int idUsuario, int idPelicula) const {
    VerticeUsuario* u = buscarVertice(idUsuario);
    if (u == nullptr) return 0;
    return buscarEstrellasEnLista(u->historial, idPelicula);
}

bool GrafoAfinidad::usuarioVioPelicula(int idUsuario, int idPelicula) const {
    return obtenerEstrellas(idUsuario, idPelicula) > 0;
}

// Inserta o actualiza manteniendo la lista ordenada ascendente por idPelicula.
// Esto permite que Jaccard corra en O(|A| + |B|) por mezcla lineal.
// Complejidad: O(V + |H|)
bool GrafoAfinidad::agregarPelicula(int idUsuario, int idPelicula, int estrellas) {
    if (estrellas <= 0)
        return eliminarPelicula(idUsuario, idPelicula);

    VerticeUsuario* u = buscarVertice(idUsuario);
    if (u == nullptr) return false;

    // Caso 1: lista vacia o el nuevo ID va antes de la cabeza
    if (u->historial == nullptr || idPelicula < u->historial->idPelicula) {
        NodoPelicula* nueva = new NodoPelicula;
        nueva->idPelicula = idPelicula;
        nueva->estrellas = estrellas;
        nueva->sig = u->historial;
        u->historial = nueva;
        marcarSucio();
        return true;
    }

    // Caso 2: coincide con la cabeza -> actualizar estrellas
    if (u->historial->idPelicula == idPelicula) {
        u->historial->estrellas = estrellas;
        marcarSucio();
        return true;
    }

    // Caso 3: avanzar hasta la posicion ordenada correspondiente
    NodoPelicula* actual = u->historial;
    while (actual->sig != nullptr && actual->sig->idPelicula < idPelicula) {
        actual = actual->sig;
    }

    if (actual->sig != nullptr && actual->sig->idPelicula == idPelicula) {
        actual->sig->estrellas = estrellas;
        marcarSucio();
        return true;
    }

    NodoPelicula* nueva = new NodoPelicula;
    nueva->idPelicula = idPelicula;
    nueva->estrellas = estrellas;
    nueva->sig = actual->sig;
    actual->sig = nueva;
    marcarSucio();
    return true;
}

// Quita una pelicula del historial del usuario y libera el nodo.
// Se usa cuando el usuario deshace una calificacion (convencion 0 estrellas).
// Complejidad: O(V + |H|)
bool GrafoAfinidad::eliminarPelicula(int idUsuario, int idPelicula) {
    VerticeUsuario* u = buscarVertice(idUsuario);
    if (u == nullptr || u->historial == nullptr)
        return false;

    if (u->historial->idPelicula == idPelicula) {
        NodoPelicula* borrar = u->historial;
        u->historial = borrar->sig;
        delete borrar;
        marcarSucio();
        return true;
    }

    NodoPelicula* actual = u->historial;
    while (actual->sig != nullptr && actual->sig->idPelicula < idPelicula) {
        actual = actual->sig;
    }

    if (actual->sig != nullptr && actual->sig->idPelicula == idPelicula) {
        NodoPelicula* borrar = actual->sig;
        actual->sig = borrar->sig;
        delete borrar;
        marcarSucio();
        return true;
    }

    return false;
}

const NodoPelicula* GrafoAfinidad::siguienteGustada(const NodoPelicula* nodo) {
    while (nodo != nullptr && nodo->estrellas < UMBRAL_GUSTO)
        nodo = nodo->sig;
    return nodo;
}

// Calcula Jaccard sobre peliculas gustadas (estrellas >= 4) usando mezcla lineal
// sobre dos listas ordenadas por idPelicula. Complejidad: O(|A| + |B|)
double GrafoAfinidad::calcularJaccardVertices(const VerticeUsuario* uA,
                                              const VerticeUsuario* uB,
                                              int& tamInterseccion,
                                              int& tamUnion) const {
    tamInterseccion = 0;
    tamUnion = 0;
    if (uA == nullptr || uB == nullptr)
        return 0.0;

    const NodoPelicula* pA = siguienteGustada(uA->historial);
    const NodoPelicula* pB = siguienteGustada(uB->historial);

    while (pA != nullptr && pB != nullptr) {
        pasosRecorrido++;
        if (pA->idPelicula < pB->idPelicula) {
            tamUnion++;
            pA = siguienteGustada(pA->sig);
        } else if (pB->idPelicula < pA->idPelicula) {
            tamUnion++;
            pB = siguienteGustada(pB->sig);
        } else {
            tamInterseccion++;
            tamUnion++;
            pA = siguienteGustada(pA->sig);
            pB = siguienteGustada(pB->sig);
        }
    }

    while (pA != nullptr) {
        pasosRecorrido++;
        tamUnion++;
        pA = siguienteGustada(pA->sig);
    }

    while (pB != nullptr) {
        pasosRecorrido++;
        tamUnion++;
        pB = siguienteGustada(pB->sig);
    }

    if (tamUnion == 0)
        return 0.0;

    return (double)tamInterseccion / (double)tamUnion;
}

double GrafoAfinidad::calcularJaccard(int idA, int idB,
                                      int& tamInterseccion, int& tamUnion) const {
    VerticeUsuario* uA = buscarVertice(idA);
    VerticeUsuario* uB = buscarVertice(idB);
    return calcularJaccardVertices(uA, uB, tamInterseccion, tamUnion);
}

void GrafoAfinidad::insertarAristaDirigida(VerticeUsuario* origen,
                                           VerticeUsuario* destino,
                                           double similitud,
                                           int interseccion,
                                           int unionTotal) {
    Arista* nueva = new Arista;
    nueva->idDestino = destino->id;
    nueva->destino = destino;
    nueva->similitud = similitud;
    nueva->interseccion = interseccion;
    nueva->unionTotal = unionTotal;
    nueva->sig = origen->adyacentes;
    origen->adyacentes = nueva;
}

// Construye el grafo disperso: solo crea aristas cuando hay afinidad real (> 0).
// Complejidad: O(V^2 * (|H_a| + |H_b|))
void GrafoAfinidad::construirRedAfinidad() {
    limpiarAristas();
    pasosRecorrido = 0;

    VerticeUsuario* uA = cabeza;
    while (uA != nullptr) {
        VerticeUsuario* uB = uA->sig;
        while (uB != nullptr) {
            int inter = 0, uni = 0;
            double jaccard = calcularJaccardVertices(uA, uB, inter, uni);

            if (jaccard > 0.0) {
                insertarAristaDirigida(uA, uB, jaccard, inter, uni);
                insertarAristaDirigida(uB, uA, jaccard, inter, uni);
                numAristas++;
            }
            uB = uB->sig;
        }
        uA = uA->sig;
    }

    sucio = false;
}

// Busca la arista entre idOrigen e idDestino. Devuelve nullptr si afinidad es 0.
// Complejidad: O(V + grado(idOrigen))
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

// Criterio determinista para desempatar vecinos: mayor similitud, luego menor ID.
bool GrafoAfinidad::mejorVecino(double simA, int idA, double simB, int idB) {
    if (simA != simB)
        return simA > simB;
    return idA < idB;
}

// Selecciona y guarda en cache los k vecinos mas afines a idUsuario.
// Se recalcula solo cuando cambia el usuario consultado o el grafo se ensucia.
// Complejidad: O(V * k) con k pequeno (k = 10), amortizado O(1) en consultas repetidas.
void GrafoAfinidad::actualizarCacheVecinos(int idUsuario, int k) {
    asegurarActualizado();

    if (k <= 0) {
        tamCache = 0;
        usuarioCache = idUsuario;
        kCache = 0;
        return;
    }

    if (usuarioCache == idUsuario && kCache == k)
        return;

    if (capacidadCache < k) {
        delete[] cacheVecinos;
        cacheVecinos = new VecinoCache[k];
        capacidadCache = k;
    }

    tamCache = 0;
    usuarioCache = idUsuario;
    kCache = k;

    VerticeUsuario* origen = buscarVertice(idUsuario);
    if (origen == nullptr)
        return;

    Arista* a = origen->adyacentes;
    while (a != nullptr) {
        if (a->similitud > 0.0 && a->destino != nullptr) {
            int pos = tamCache;
            if (tamCache < k) {
                tamCache++;
            } else if (mejorVecino(a->similitud, a->idDestino,
                                   cacheVecinos[tamCache - 1].similitud,
                                   cacheVecinos[tamCache - 1].usuario->id)) {
                pos = tamCache - 1;
            } else {
                a = a->sig;
                continue;
            }

            while (pos > 0 && mejorVecino(a->similitud, a->idDestino,
                                          cacheVecinos[pos - 1].similitud,
                                          cacheVecinos[pos - 1].usuario->id)) {
                cacheVecinos[pos] = cacheVecinos[pos - 1];
                pos--;
            }

            cacheVecinos[pos].usuario = a->destino;
            cacheVecinos[pos].similitud = a->similitud;
        }
        a = a->sig;
    }
}

// Formula propuesta: sum(sim(v) * (estrellas_v - 1) / 4) / sum(sim(v))
// sobre los vecinos del cache que calificaron idPelicula. Devuelve 0 si ninguno la califico.
// Complejidad por llamada con cache caliente: O(k * |H_v|)
double GrafoAfinidad::calcularPuntajeColaborativo(int idUsuario, int idPelicula, int k) {
    actualizarCacheVecinos(idUsuario, k);
    if (tamCache == 0)
        return 0.0;

    double numerador = 0.0;
    double denominador = 0.0;

    for (int i = 0; i < tamCache; i++) {
        int estrellas = buscarEstrellasEnLista(cacheVecinos[i].usuario->historial, idPelicula);
        if (estrellas > 0) {
            double notaNorm = (double)(estrellas - 1) / 4.0; // 1->0.0, 3->0.5, 5->1.0
            numerador += cacheVecinos[i].similitud * notaNorm;
            denominador += cacheVecinos[i].similitud;
        }
    }

    if (denominador <= 0.0)
        return 0.0;

    return numerador / denominador;
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

int GrafoAfinidad::tamanoCacheVecinos() const {
    return tamCache;
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

string GrafoAfinidad::estadoAdyacenciaUsuario(int idUsuario) const {
    ostringstream o;
    o << fixed << setprecision(2);
    VerticeUsuario* v = buscarVertice(idUsuario);
    if (v == nullptr)
        return "U" + to_string(idUsuario) + "->[]";

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
    return o.str();
}