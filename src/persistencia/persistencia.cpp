#include "modulos.h"
#include "persistencia.h"
#include "Gestor_Perfil.h"
#include "recomendador.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

// =====================================================================
// Integrante 5 (Edrian): Persistencia (PR 5 del plan)
//
// Archivos (en data/):
//   usuarios.txt        ID,Nombre
//   calificaciones.txt  IDUsuario,IDPelicula,Estrellas
//
// En memoria se guardan dos listas enlazadas simples (punteros, new/delete):
//   - usuarios: id y nombre de cada usuario.
//   - pendientes: las Calificacion de los usuarios NO activos. Las del activo
//     las tiene el Gestor de Perfiles (Int.2) en su lista doble.
// Al guardar se escriben "pendientes" + calificacionesDelActivo().
// Cada calificacion, al cargarse, tambien se reparte a:
//   registrarVoto (Int.1, rating y votos)  y  grafoRegistrarCalificacion (Int.4).
// =====================================================================

struct NodoUsuario {
    int id;
    string nombre;
    NodoUsuario* sig;
};

struct NodoCalificacion {
    Calificacion dato;
    NodoCalificacion* sig;
};

static NodoUsuario* usuarios = nullptr;
static NodoUsuario* finUsuarios = nullptr;
static int numUsuarios = 0;

static NodoCalificacion* pendientes = nullptr;
static NodoCalificacion* finPendientes = nullptr;
static int numCargadas = 0;

static string rutaUsuarios = "data/usuarios.txt";
static string rutaCalificaciones = "data/calificaciones.txt";

// ================= AUXILIARES =================

// Busca el archivo tanto si el programa se ejecuta desde la raiz del
// repositorio como desde una subcarpeta (por ejemplo src/).
static string rutaDatos(const string& archivo) {
    string candidatas[] = { "data/" + archivo, "../data/" + archivo, "../../data/" + archivo };
    for (int i = 0; i < 3; i++) {
        ifstream f(candidatas[i].c_str());
        if (f.is_open()) return candidatas[i];
    }
    return "data/" + archivo;
}

static string sinRetornoCarro(const string& s) {
    if (!s.empty() && s[s.size() - 1] == '\r')
        return s.substr(0, s.size() - 1);
    return s;
}

// Entero estricto: solo digitos (con espacios alrededor). false si no lo es.
static bool aEntero(const string& texto, int& valor) {
    size_t i = 0;
    while (i < texto.size() && texto[i] == ' ') i++;
    size_t inicio = i;
    while (i < texto.size() && texto[i] >= '0' && texto[i] <= '9') i++;
    size_t fin = i;
    while (i < texto.size() && texto[i] == ' ') i++;
    if (fin == inicio || i != texto.size() || fin - inicio > 9)
        return false;
    valor = atoi(texto.substr(inicio, fin - inicio).c_str());
    return true;
}

// Busqueda lineal en la lista de usuarios. O(U)
static NodoUsuario* buscarUsuario(int id) {
    NodoUsuario* u = usuarios;
    while (u != nullptr) {
        if (u->id == id) return u;
        u = u->sig;
    }
    return nullptr;
}

// Inserta al final para conservar el orden del archivo. O(1) con puntero al final
static NodoUsuario* agregarUsuario(int id, const string& nombre) {
    NodoUsuario* nuevo = new NodoUsuario;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->sig = nullptr;
    if (finUsuarios == nullptr) usuarios = nuevo;
    else finUsuarios->sig = nuevo;
    finUsuarios = nuevo;
    numUsuarios++;
    return nuevo;
}

static void agregarPendiente(int idUsuario, int idPelicula, int estrellas) {
    NodoCalificacion* nuevo = new NodoCalificacion;
    nuevo->dato.idUsuario = idUsuario;
    nuevo->dato.idPelicula = idPelicula;
    nuevo->dato.estrellas = estrellas;
    nuevo->sig = nullptr;
    if (finPendientes == nullptr) pendientes = nuevo;
    else finPendientes->sig = nuevo;
    finPendientes = nuevo;
}

// Estrellas de (usuario, pelicula) en la lista de pendientes; 0 si no esta. O(P)
static NodoCalificacion* buscarPendiente(int idUsuario, int idPelicula) {
    NodoCalificacion* c = pendientes;
    while (c != nullptr) {
        if (c->dato.idUsuario == idUsuario && c->dato.idPelicula == idPelicula)
            return c;
        c = c->sig;
    }
    return nullptr;
}

static void liberarListas() {
    while (usuarios != nullptr) {
        NodoUsuario* b = usuarios;
        usuarios = usuarios->sig;
        delete b;
    }
    finUsuarios = nullptr;
    numUsuarios = 0;
    while (pendientes != nullptr) {
        NodoCalificacion* b = pendientes;
        pendientes = pendientes->sig;
        delete b;
    }
    finPendientes = nullptr;
}

// Libera las listas al terminar el programa (cada new con su delete).
static struct GuardiaPersistencia {
    ~GuardiaPersistencia() { liberarListas(); }
} guardia;

// Copia las calificaciones del activo (Int.2) a un arreglo dinamico que crece al
// doble si no alcanza. El llamador debe hacer delete[]. Devuelve cuantas hay.
static int copiarCalificacionesActivo(Calificacion*& buffer) {
    int capacidad = 64;
    while (true) {
        buffer = new Calificacion[capacidad];
        int n = calificacionesDelActivo(buffer, capacidad);
        if (n < capacidad) return n;
        delete[] buffer;
        capacidad *= 2;
    }
}

// ================= CARGA =================

static void cargarUsuarios() {
    ifstream f(rutaUsuarios.c_str());
    if (!f.is_open()) return;
    string linea;
    while (getline(f, linea)) {
        linea = sinRetornoCarro(linea);
        size_t coma = linea.find(',');
        if (coma == string::npos) continue;
        int id;
        if (!aEntero(linea.substr(0, coma), id) || id <= 0) continue;   // encabezado
        if (buscarUsuario(id) == nullptr)
            agregarUsuario(id, linea.substr(coma + 1));
    }
}

static void cargarCalificaciones() {
    ifstream f(rutaCalificaciones.c_str());
    if (!f.is_open()) return;
    int activo = usuarioActivo();
    string linea;
    while (getline(f, linea)) {
        linea = sinRetornoCarro(linea);
        stringstream ss(linea);
        string sU, sP, sE;
        getline(ss, sU, ',');
        getline(ss, sP, ',');
        getline(ss, sE, ',');
        int u, p, e;
        if (!aEntero(sU, u) || !aEntero(sP, p) || !aEntero(sE, e)) continue;  // encabezado
        if (u <= 0 || e < 1 || e > 5 || obtenerPelicula(p) == nullptr) continue;

        if (buscarUsuario(u) == nullptr)
            agregarUsuario(u, "Usuario " + to_string(u));

        // Si la fila repite (usuario, pelicula) se toma como recalificacion,
        // asi el rating de la pelicula no cuenta dos votos del mismo usuario.
        int anteriores = 0;
        if (u == activo) {
            anteriores = estrellasDelActivo(p);
            perfilCargarCalificacion(p, e);
        } else {
            NodoCalificacion* previa = buscarPendiente(u, p);
            if (previa != nullptr) {
                anteriores = previa->dato.estrellas;
                previa->dato.estrellas = e;
            } else {
                agregarPendiente(u, p, e);
            }
        }
        registrarVoto(p, e, anteriores);
        grafoRegistrarCalificacion(u, p, e);
        if (anteriores == 0) numCargadas++;
    }
}

bool cargarDatos() {
    if (!cargarCatalogo(rutaDatos("peliculas.txt")))
        return false;
    rutaUsuarios = rutaDatos("usuarios.txt");
    rutaCalificaciones = rutaDatos("calificaciones.txt");
    cargarUsuarios();
    if (buscarUsuario(usuarioActivo()) == nullptr)
        agregarUsuario(usuarioActivo(), "Usuario " + to_string(usuarioActivo()));
    cargarCalificaciones();
    return true;
}

// ================= GUARDADO =================

bool guardarDatos() {
    ofstream fu(rutaUsuarios.c_str());
    if (!fu.is_open()) return false;
    fu << "ID,Nombre\n";
    for (NodoUsuario* u = usuarios; u != nullptr; u = u->sig)
        fu << u->id << "," << u->nombre << "\n";
    fu.close();

    ofstream fc(rutaCalificaciones.c_str());
    if (!fc.is_open()) return false;
    fc << "IDUsuario,IDPelicula,Estrellas\n";

    // Se escribe agrupado por usuario (en el orden de usuarios.txt) para que el
    // archivo sea facil de leer. O(U * (P + H)), suficiente para ~1500 filas.
    int activo = usuarioActivo();
    Calificacion* propias = nullptr;
    int nPropias = copiarCalificacionesActivo(propias);
    for (NodoUsuario* u = usuarios; u != nullptr; u = u->sig) {
        if (u->id == activo) {
            for (int i = nPropias - 1; i >= 0; i--)   // historial: de mas antigua a mas nueva
                fc << activo << "," << propias[i].idPelicula << "," << propias[i].estrellas << "\n";
        } else {
            for (NodoCalificacion* c = pendientes; c != nullptr; c = c->sig)
                if (c->dato.idUsuario == u->id)
                    fc << c->dato.idUsuario << "," << c->dato.idPelicula << ","
                       << c->dato.estrellas << "\n";
        }
    }
    delete[] propias;
    return true;
}

// ================= CAMBIO DE USUARIO =================

string cambiarUsuario(int idUsuario) {
    if (idUsuario <= 0)
        return "ERROR: el ID de usuario debe ser mayor que 0.";
    int anterior = usuarioActivo();
    if (idUsuario == anterior)
        return "Ya eres el usuario " + to_string(idUsuario) + " (" + nombreUsuario(idUsuario) + ").";

    bool nuevo = false;
    if (buscarUsuario(idUsuario) == nullptr) {
        agregarUsuario(idUsuario, "Usuario " + to_string(idUsuario));
        nuevo = true;
    }

    // 1. Las calificaciones del activo actual pasan a la lista de pendientes
    Calificacion* propias = nullptr;
    int nPropias = copiarCalificacionesActivo(propias);
    for (int i = nPropias - 1; i >= 0; i--)
        agregarPendiente(anterior, propias[i].idPelicula, propias[i].estrellas);
    delete[] propias;

    // 2. El Gestor de Perfiles limpia su memoria y cambia de usuario
    perfilCambiarUsuario(idUsuario);

    // 3. Se sacan de la lista las calificaciones del nuevo activo. O(P)
    int cargadas = 0;
    NodoCalificacion* previo = nullptr;
    NodoCalificacion* c = pendientes;
    while (c != nullptr) {
        NodoCalificacion* siguiente = c->sig;
        if (c->dato.idUsuario == idUsuario) {
            perfilCargarCalificacion(c->dato.idPelicula, c->dato.estrellas);
            cargadas++;
            if (previo == nullptr) pendientes = siguiente;
            else previo->sig = siguiente;
            if (c == finPendientes) finPendientes = previo;
            delete c;
        } else {
            previo = c;
        }
        c = siguiente;
    }

    // La ultima lista de 'explicar' era del usuario anterior
    recomendadorReiniciar();

    ostringstream o;
    o << "Usuario activo: " << idUsuario << " - " << nombreUsuario(idUsuario)
      << " (" << cargadas << " calificacion" << (cargadas == 1 ? "" : "es") << ")";
    if (nuevo)
        o << ". Usuario nuevo: califica peliculas para personalizar tus recomendaciones.";
    if (modoDevActivo())
        o << "\n[DEV] Persistencia: " << numUsuarios << " usuarios en lista, "
          << nPropias << " calificaciones movidas a pendientes, " << cargadas << " recuperadas";
    return o.str();
}

// ================= CONSULTAS =================

int totalUsuarios() {
    return numUsuarios;
}

int totalCalificacionesCargadas() {
    return numCargadas;
}

string nombreUsuario(int idUsuario) {
    NodoUsuario* u = buscarUsuario(idUsuario);
    return u != nullptr ? u->nombre : "";
}
