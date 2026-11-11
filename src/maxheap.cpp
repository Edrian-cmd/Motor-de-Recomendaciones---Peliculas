#include "maxheap.h"
#include <sstream>
#include <iomanip>

using namespace std;

MaxHeap::MaxHeap(int capacidadInicial)
    : datos(nullptr), cantidad(0),
      capacidad(capacidadInicial > 0 ? capacidadInicial : 1),
      numIntercambios(0) {
    datos = new Candidato[capacidad];
}

MaxHeap::~MaxHeap() {
    delete[] datos;
}

// Empate de puntaje: gana el id menor, asi el ranking siempre sale igual.
bool MaxHeap::mayor(const Candidato& a, const Candidato& b) {
    if (a.puntaje != b.puntaje)
        return a.puntaje > b.puntaje;
    return a.id < b.id;
}

void MaxHeap::crecer() {
    int nuevaCapacidad = capacidad * 2;
    Candidato* nuevo = new Candidato[nuevaCapacidad];
    for (int i = 0; i < cantidad; i++)
        nuevo[i] = datos[i];
    delete[] datos;
    datos = nuevo;
    capacidad = nuevaCapacidad;
}

void MaxHeap::intercambiar(int i, int j) {
    Candidato tmp = datos[i];
    datos[i] = datos[j];
    datos[j] = tmp;
    numIntercambios++;
}

void MaxHeap::flotar(int i) {
    while (i > 0) {
        int padre = (i - 1) / 2;
        if (!mayor(datos[i], datos[padre]))
            break;
        intercambiar(i, padre);
        i = padre;
    }
}

void MaxHeap::hundir(int i) {
    while (true) {
        int izq = 2 * i + 1;
        int der = 2 * i + 2;
        int elegido = i;
        if (izq < cantidad && mayor(datos[izq], datos[elegido])) elegido = izq;
        if (der < cantidad && mayor(datos[der], datos[elegido])) elegido = der;
        if (elegido == i)
            break;
        intercambiar(i, elegido);
        i = elegido;
    }
}

void MaxHeap::insertar(const Candidato& c) {
    if (cantidad == capacidad)
        crecer();
    datos[cantidad] = c;
    cantidad++;
    flotar(cantidad - 1);
}

Candidato MaxHeap::extraerMax() {
    Candidato raiz = datos[0];
    cantidad--;
    if (cantidad > 0) {
        datos[0] = datos[cantidad];
        hundir(0);
    }
    return raiz;
}

bool MaxHeap::vacio() const {
    return cantidad == 0;
}

int MaxHeap::tamanio() const {
    return cantidad;
}

int MaxHeap::intercambios() const {
    return numIntercambios;
}

string MaxHeap::estado() const {
    ostringstream o;
    o << fixed << setprecision(1) << "[";
    for (int i = 0; i < cantidad; i++) {
        if (i > 0) o << ", ";
        o << datos[i].id << ":" << datos[i].puntaje;
    }
    o << "]";
    return o.str();
}
