#ifndef GESTORPERFIL_H
#define GESTORPERFIL_H

#include <iostream>
#include <string>

using namespace std;

struct Historial {
    int id_pelicula;
    string accion;
    Historial* next;
    Historial* back;
};

struct Pila {
    string descripcion_accion;
    Pila* next;
};

struct Cola {
    int id_pelicula;
    Cola* next;
};

void agregarAlHistorial(Historial*& head, int id, string accion);
void mostrarHistorial(Historial* head);

void registrarAccion(Pila*& tope, string accion);
void deshacerUltimaAccion(Pila*& tope);

void encolarPelicula(Cola*& frente, Cola*& final, int id);
void mostrarSiguienteDescubrimiento(Cola*& frente);

void cargarMocksGestorPerfiles(Historial*& cabeza, Pila*& tope, Cola*& frente, Cola*& final);

#endif