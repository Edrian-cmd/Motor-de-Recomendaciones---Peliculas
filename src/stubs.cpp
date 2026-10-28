#include "modulos.h"
#include <sstream>

using namespace std;

// TEMPORAL: stubs de las funciones del contrato que aun no tienen modulo real.
// Cuando un integrante entregue la suya, se BORRA de aqui.

static string num(int n) {
    ostringstream o;
    o << n;
    return o.str();
}

// ---- Integrante 1 (falta buscarPorAnio) ----
string buscarPorAnio(int desde, int hasta) {
    return "[STUB Int.1] buscarPorAnio(" + num(desde) + ", " + num(hasta) + ") - modulo AVL en construccion";
}

// ---- Integrante 2 (faltan las 5 funciones del contrato) ----
string calificar(int id, int estrellas) {
    return "[STUB Int.2] calificar(" + num(id) + ", " + num(estrellas) + ") - modulo Perfiles en construccion";
}
string deshacer() {
    return "[STUB Int.2] deshacer() - modulo Pila en construccion";
}
string verHistorial() {
    return "[STUB Int.2] verHistorial() - modulo Lista doble en construccion";
}
string descubrir() {
    return "[STUB Int.2] descubrir() - modulo Cola en construccion";
}
string siguiente() {
    return "[STUB Int.2] siguiente() - modulo Cola en construccion";
}