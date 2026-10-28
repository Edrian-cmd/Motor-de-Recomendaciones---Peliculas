#include "modulos.h"
#include "recomendador.h"
#include "maxheap.h"
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;

// ================= DATOS DE PRUEBA (MOCK) =================
// TEMPORAL (Entregable 2): catalogo y perfil escritos a mano para probar el
// Max-Heap sin depender de los modulos de los Integrantes 1 y 2.
// En el Entregable 3 se reemplazan por el Hash (Int.1) y el perfil (Int.2).

struct PeliculaMock {
    int id;
    string titulo;
    string genero;
    int anio;
    double ratingPromedio;      // 1.0 a 5.0, promedio de la comunidad
};

struct CalificacionMock {
    int id;
    int estrellas;              // 1 a 5
};

static const PeliculaMock CATALOGO[] = {
    {101, "El Padrino",                 "Drama",           1972, 4.8},
    {102, "Mad Max: Furia en el Camino","Accion",          2015, 4.3},
    {103, "Toy Story",                  "Animacion",       1995, 4.5},
    {104, "Interestelar",               "Ciencia Ficcion", 2014, 4.6},
    {105, "El Conjuro",                 "Terror",          2013, 3.9},
    {106, "Superbad",                   "Comedia",         2007, 3.7},
    {107, "Duro de Matar",              "Accion",          1988, 4.2},
    {108, "Matrix",                     "Ciencia Ficcion", 1999, 4.7},
    {109, "Forrest Gump",               "Drama",           1994, 4.6},
    {110, "Coco",                       "Animacion",       2017, 4.5},
    {111, "John Wick",                  "Accion",          2014, 4.1},
    {112, "El Resplandor",              "Terror",          1980, 4.3},
    {113, "Blade Runner 2049",          "Ciencia Ficcion", 2017, 4.2},
    {114, "Que Paso Ayer",              "Comedia",         2009, 3.8},
    {115, "Gladiador",                  "Accion",          2000, 4.5},
    {116, "Volver al Futuro",           "Ciencia Ficcion", 1985, 4.6},
};
static const int TOTAL_PELICULAS = sizeof(CATALOGO) / sizeof(CATALOGO[0]);

// Perfil del usuario activo: le gusta la ciencia ficcion, no el terror.
static const CalificacionMock PERFIL[] = {
    {108, 5}, {104, 5}, {107, 4}, {105, 1}, {106, 2}, {101, 3},
};
static const int TOTAL_CALIFICACIONES = sizeof(PERFIL) / sizeof(PERFIL[0]);

// ================= ESTADO DEL MODULO =================

static const double PESO_GENERO  = 0.6;
static const double PESO_CALIDAD = 0.4;

static bool modoDev = false;

// Ultima lista entregada por recomendar(), la usa explicar().
static Candidato* ultimaLista = nullptr;
static int tamUltimaLista = 0;
static string filtroUltimaLista;

// Libera ultimaLista al terminar el programa.
static struct LiberadorUltimaLista {
    ~LiberadorUltimaLista() { delete[] ultimaLista; }
} liberador;

void recomendadorModoDev(bool activo) {
    modoDev = activo;
}

// ================= AUXILIARES =================

static string minusculas(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.size(); i++)
        r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

// Con el mock es una busqueda lineal; al integrar se usa el Hash O(1) del Int.1.
static const PeliculaMock* obtenerPelicula(int id) {
    for (int i = 0; i < TOTAL_PELICULAS; i++)
        if (CATALOGO[i].id == id)
            return &CATALOGO[i];
    return nullptr;
}

static bool yaCalificada(int id) {
    for (int i = 0; i < TOTAL_CALIFICACIONES; i++)
        if (PERFIL[i].id == id)
            return true;
    return false;
}

// Afinidad del usuario con un genero, en [-1, 1]:
// promedio de (estrellas - 3) / 2 sobre las peliculas de ese genero que califico.
// 5 estrellas -> +1, 3 -> 0, 1 -> -1. Sin calificaciones del genero -> 0 (neutral).
static double afinidadGenero(const string& genero, int& usadas) {
    double suma = 0;
    usadas = 0;
    string g = minusculas(genero);
    for (int i = 0; i < TOTAL_CALIFICACIONES; i++) {
        const PeliculaMock* p = obtenerPelicula(PERFIL[i].id);
        if (p != nullptr && minusculas(p->genero) == g) {
            suma += (PERFIL[i].estrellas - 3) / 2.0;
            usadas++;
        }
    }
    return usadas == 0 ? 0.0 : suma / usadas;
}

// puntaje = 100 * (0.6 * genero + 0.4 * calidad), ambos componentes en [0, 1]
static Candidato puntuar(const PeliculaMock& p) {
    int usadas;
    Candidato c;
    c.id = p.id;
    c.compGenero = (afinidadGenero(p.genero, usadas) + 1.0) / 2.0;
    c.compCalidad = p.ratingPromedio / 5.0;
    c.puntaje = 100.0 * (PESO_GENERO * c.compGenero + PESO_CALIDAD * c.compCalidad);
    return c;
}

// ================= CONTRATO (modulos.h) =================

string recomendar(int n, const string& genero) {
    if (n <= 0)
        return "ERROR: el top debe ser mayor que 0.";

    string filtro = minusculas(genero);
    MaxHeap heap;
    for (int i = 0; i < TOTAL_PELICULAS; i++) {
        const PeliculaMock& p = CATALOGO[i];
        if (yaCalificada(p.id))
            continue;
        if (!filtro.empty() && minusculas(p.genero) != filtro)
            continue;
        heap.insertar(puntuar(p));
    }

    if (heap.vacio()) {
        if (filtro.empty())
            return "No hay peliculas nuevas para recomendar.";
        return "No hay peliculas nuevas del genero \"" + genero + "\".";
    }

    ostringstream o;
    o << fixed << setprecision(1);

    int insertados = heap.tamanio();
    int intercambiosInsercion = heap.intercambios();
    if (modoDev) {
        o << "[DEV] Max-Heap: " << insertados << " candidatos insertados, "
          << intercambiosInsercion << " intercambios al flotar\n";
        o << "[DEV] arreglo del heap (id:puntaje): " << heap.estado() << "\n";
    }

    int k = n < insertados ? n : insertados;
    delete[] ultimaLista;
    ultimaLista = new Candidato[k];
    tamUltimaLista = k;
    filtroUltimaLista = genero;
    for (int i = 0; i < k; i++)
        ultimaLista[i] = heap.extraerMax();

    if (modoDev) {
        o << "[DEV] " << k << " extracciones, "
          << heap.intercambios() - intercambiosInsercion << " intercambios al hundir, "
          << heap.tamanio() << " candidatos sin extraer\n";
    }

    o << "Top " << k << " recomendaciones";
    if (!filtro.empty())
        o << " de " << genero;
    o << ":";
    if (k < n)
        o << " (solo hay " << k << " candidatas)";
    for (int i = 0; i < k; i++) {
        const PeliculaMock* p = obtenerPelicula(ultimaLista[i].id);
        o << "\n  " << (i + 1) << ". [" << p->id << "] " << p->titulo
          << " (" << p->anio << ") - " << p->genero
          << " - puntaje " << ultimaLista[i].puntaje;
    }
    return o.str();
}

string explicar(int id) {
    if (ultimaLista == nullptr)
        return "ERROR: primero usa 'recomendar top <n>'.";

    int pos = -1;
    for (int i = 0; i < tamUltimaLista; i++)
        if (ultimaLista[i].id == id)
            pos = i;
    if (pos == -1) {
        ostringstream e;
        e << "ERROR: la pelicula " << id << " no esta en la ultima recomendacion.";
        return e.str();
    }

    const Candidato& c = ultimaLista[pos];
    const PeliculaMock* p = obtenerPelicula(id);
    int usadas;
    double afinidad = afinidadGenero(p->genero, usadas);

    ostringstream o;
    o << fixed << setprecision(1);
    o << "[" << p->id << "] " << p->titulo << " - puesto " << (pos + 1)
      << " de " << tamUltimaLista << "\n";
    if (usadas == 0)
        o << "  Genero " << p->genero << ": no calificaste ninguna, se toma neutral\n";
    else
        o << "  Genero " << p->genero << ": afinidad " << showpos << setprecision(2)
          << afinidad << noshowpos << setprecision(1) << " (segun " << usadas
          << " calificacion" << (usadas == 1 ? "" : "es") << " tuya"
          << (usadas == 1 ? "" : "s") << ")\n";
    o << "  Componente genero : " << setprecision(2) << c.compGenero << " x "
      << (int)(PESO_GENERO * 100 + 0.5) << "% = " << setprecision(1)
      << 100.0 * PESO_GENERO * c.compGenero << " pts\n";
    o << "  Componente calidad: " << p->ratingPromedio << "/5 x "
      << (int)(PESO_CALIDAD * 100 + 0.5) << "% = "
      << 100.0 * PESO_CALIDAD * c.compCalidad << " pts\n";
    o << "  Puntaje total     : " << c.puntaje << " / 100";
    return o.str();
}
