#include "modulos.h"
#include <sstream>

using namespace std;

// Archivo TEMPORAL: cuando un integrante entregue su modulo real,
// se borra su seccion de aqui (si no, hay "multiple definition").

static string num(int n) {
    ostringstream o;
    o << n;
    return o.str();
}

// ---- Integrante 1 ----
bool cargarCatalogo(const string& ruta) {
    return true;
}
string buscarPorId(int id) {
    return "[STUB Int.1] buscarPorId(" + num(id) + ") - modulo Hash en construccion";
}
string buscarPorGenero(const string& genero) {
    return "[STUB Int.1] buscarPorGenero(\"" + genero + "\") - modulo AVL en construccion";
}
string buscarPorAnio(int desde, int hasta) {
    return "[STUB Int.1] buscarPorAnio(" + num(desde) + ", " + num(hasta) + ") - modulo AVL en construccion";
}

// ---- Integrante 2 ----
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

// ---- Integrante 3 ----
string recomendar(int n, const string& genero) {
    return "[STUB Int.3] recomendar(" + num(n) + ", \"" + genero + "\") - modulo Max-Heap en construccion";
}
string explicar(int id) {
    return "[STUB Int.3] explicar(" + num(id) + ") - modulo Max-Heap en construccion";
}

// ---- Integrante 4 ----
string afinidad(int usuario) {
    return "[STUB Int.4] afinidad(" + num(usuario) + ") - modulo Grafos en construccion";
}