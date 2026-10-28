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


