#include "modulos.h"
#include "modelo.h"
#include "recomendador.h"
#include "maxheap.h"
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;

// =====================================================================
// Integrante 3: Matematico (Motor de Recomendacion Personalizado)
//
// Ya no usa datos mock. Depende de estas funciones del contrato v2:
//   Int.1 Aaron : obtenerPelicula, totalPeliculas, peliculaPorIndice
//                 (y los campos ratingPromedio / numVotos de Pelicula)
//   Int.2 Imanol: estrellasDelActivo, calificacionesDelActivo
//   Int.4 Oliver: puntajeColaborativo
//   Int.5 Edrian: modoDevActivo
//
// Puntaje de cada pelicula candidata (0 a 100):
//   puntaje = 100 * ( 0.45 * genero        afinidad del usuario con el genero
//                   + 0.20 * calidad       promedio bayesiano de estrellas / 5
//                   + 0.10 * popularidad   numVotos / maximo de votos
//                   + 0.25 * colaborativo) lo que opinan los usuarios afines
//
// Costo de recomendar top N sobre m peliculas del catalogo:
//   perfil de generos O(h * g)  (h = calificaciones del usuario, g = generos)
//   puntuar e insertar O(m log m) en el Max-Heap
//   extraer el Top-N  O(N log m)
//   Nunca se ordena el catalogo completo (sin Bubble Sort ni QuickSort).
// =====================================================================

static const double PESO_GENERO       = 0.45;
static const double PESO_CALIDAD      = 0.20;
static const double PESO_POPULARIDAD  = 0.10;
static const double PESO_COLABORATIVO = 0.25;

// Promedio bayesiano (el mismo criterio del ranking de IMDb):
//   calidad = (v * R + M * C) / (v + M)
//   v = votos de la pelicula, R = su promedio, C = promedio de todo el catalogo.
// Una pelicula con 1 solo voto de 5 estrellas no le gana a una con 200 votos de
// 4.6: con pocos votos su nota se "jala" hacia el promedio general C.
// M dice cuantos votos hacen falta para confiar en el promedio propio.
static const double VOTOS_CONFIANZA = 5.0;
static const double PROMEDIO_NEUTRO = 3.0;  // C si nadie califico nada aun

// ================= ESTADO DEL MODULO =================

static bool modoDevLocal = false;

void recomendadorModoDev(bool activo) {
    modoDevLocal = activo;
}

static bool devActivo() {
    return modoDevLocal || modoDevActivo();
}

// Ultima lista entregada por recomendar(), la usa explicar().
static Candidato* ultimaLista = nullptr;
static int tamUltimaLista = 0;

// Libera ultimaLista al terminar el programa.
static struct LiberadorUltimaLista {
    ~LiberadorUltimaLista() { delete[] ultimaLista; }
} liberador;

void recomendadorReiniciar() {
    delete[] ultimaLista;
    ultimaLista = nullptr;
    tamUltimaLista = 0;
}

// ================= AUXILIARES =================

static string minusculas(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.size(); i++)
        r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}

static double acotar01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

// ---------------------------------------------------------------------
// Perfil de generos del usuario activo.
// Se construye UNA vez por consulta a partir de sus calificaciones, asi no se
// recorre su historial por cada una de las m peliculas del catalogo.
// Arreglo dinamico de (genero, suma, cantidad); hay pocos generos (~13), por
// eso una busqueda lineal en el arreglo es suficiente y facil de explicar.
// Afinidad de un genero en [-1, 1]: promedio de (estrellas - 3) / 2
//   5 estrellas -> +1, 3 -> 0, 1 -> -1, sin calificaciones -> 0 (neutral).
// ---------------------------------------------------------------------
struct AfinidadGenero {
    string genero;      // en minusculas
    double suma;
    int usadas;
};

class PerfilGeneros {
public:
    PerfilGeneros() : datos(nullptr), cantidad(0), calificaciones(0) {}
    ~PerfilGeneros() { delete[] datos; }

    void construir() {
        // Pedimos las calificaciones al Int.2; si el arreglo se llena,
        // duplicamos su tamanio y volvemos a pedir.
        int capacidad = 32;
        Calificacion* buffer = nullptr;
        int n = 0;
        while (true) {
            buffer = new Calificacion[capacidad];
            n = calificacionesDelActivo(buffer, capacidad);
            if (n < capacidad) break;
            delete[] buffer;
            capacidad *= 2;
        }

        calificaciones = n;
        datos = new AfinidadGenero[n > 0 ? n : 1];   // a lo mas un genero por calificacion
        for (int i = 0; i < n; i++) {
            const Pelicula* p = obtenerPelicula(buffer[i].idPelicula);
            if (p == nullptr) continue;
            AfinidadGenero* g = buscar(minusculas(p->genero));
            if (g == nullptr) {
                g = &datos[cantidad++];
                g->genero = minusculas(p->genero);
                g->suma = 0;
                g->usadas = 0;
            }
            g->suma += (buffer[i].estrellas - 3) / 2.0;
            g->usadas++;
        }
        delete[] buffer;
    }

    double afinidad(const string& genero, int& usadas) const {
        const AfinidadGenero* g = buscarConst(minusculas(genero));
        if (g == nullptr || g->usadas == 0) {
            usadas = 0;
            return 0.0;
        }
        usadas = g->usadas;
        return g->suma / g->usadas;
    }

    int totalCalificaciones() const { return calificaciones; }
    int totalGeneros() const { return cantidad; }

private:
    AfinidadGenero* datos;
    int cantidad;
    int calificaciones;

    AfinidadGenero* buscar(const string& g) {
        for (int i = 0; i < cantidad; i++)
            if (datos[i].genero == g) return &datos[i];
        return nullptr;
    }
    const AfinidadGenero* buscarConst(const string& g) const {
        for (int i = 0; i < cantidad; i++)
            if (datos[i].genero == g) return &datos[i];
        return nullptr;
    }

    // No copiable: es duenio de su arreglo
    PerfilGeneros(const PerfilGeneros&);
    PerfilGeneros& operator=(const PerfilGeneros&);
};

// Datos globales del catalogo necesarios para normalizar: una pasada O(m).
struct EstadisticasCatalogo {
    double promedioGlobal;   // C del promedio bayesiano
    int maxVotos;            // para normalizar la popularidad
};

static EstadisticasCatalogo calcularEstadisticas() {
    EstadisticasCatalogo e;
    double sumaEstrellas = 0;
    long totalVotos = 0;
    e.maxVotos = 0;
    int m = totalPeliculas();
    for (int i = 0; i < m; i++) {
        const Pelicula* p = peliculaPorIndice(i);
        if (p == nullptr || p->numVotos <= 0) continue;
        sumaEstrellas += p->ratingPromedio * p->numVotos;
        totalVotos += p->numVotos;
        if (p->numVotos > e.maxVotos) e.maxVotos = p->numVotos;
    }
    e.promedioGlobal = totalVotos > 0 ? sumaEstrellas / totalVotos : PROMEDIO_NEUTRO;
    return e;
}

static double calidadBayesiana(const Pelicula& p, const EstadisticasCatalogo& e) {
    double v = p.numVotos > 0 ? p.numVotos : 0;
    double r = p.numVotos > 0 ? p.ratingPromedio : 0;
    return (v * r + VOTOS_CONFIANZA * e.promedioGlobal) / (v + VOTOS_CONFIANZA);
}

static Candidato puntuar(const Pelicula& p, const PerfilGeneros& perfil,
                         const EstadisticasCatalogo& e) {
    Candidato c;
    c.id = p.id;
    double afinidad = perfil.afinidad(p.genero, c.usadasGenero);
    c.compGenero       = (afinidad + 1.0) / 2.0;
    c.compCalidad      = acotar01(calidadBayesiana(p, e) / 5.0);
    c.compPopularidad  = e.maxVotos > 0 ? (double)p.numVotos / e.maxVotos : 0.0;
    c.compColaborativo = acotar01(puntajeColaborativo(p.id));
    c.puntaje = 100.0 * (PESO_GENERO       * c.compGenero
                       + PESO_CALIDAD      * c.compCalidad
                       + PESO_POPULARIDAD  * c.compPopularidad
                       + PESO_COLABORATIVO * c.compColaborativo);
    return c;
}

// Recorre el catalogo, descarta lo que el usuario ya califico y lo que no pasa
// el filtro de genero, y mete el resto al Max-Heap. Devuelve cuantas descarto
// por estar ya calificadas (dato para el modo desarrollador).
static int llenarHeap(MaxHeap& heap, const string& filtroMinusculas) {
    PerfilGeneros perfil;
    perfil.construir();
    EstadisticasCatalogo e = calcularEstadisticas();

    int yaCalificadas = 0;
    int m = totalPeliculas();
    for (int i = 0; i < m; i++) {
        const Pelicula* p = peliculaPorIndice(i);
        if (p == nullptr) continue;
        if (estrellasDelActivo(p->id) != 0) {
            yaCalificadas++;
            continue;
        }
        if (!filtroMinusculas.empty() && minusculas(p->genero) != filtroMinusculas)
            continue;
        heap.insertar(puntuar(*p, perfil, e));
    }
    return yaCalificadas;
}

// ================= CONTRATO (modulos.h) =================

string recomendar(int n, const string& genero) {
    if (n <= 0)
        return "ERROR: el top debe ser mayor que 0.";
    if (totalPeliculas() <= 0)
        return "ERROR: el catalogo esta vacio.";

    string filtro = minusculas(genero);
    MaxHeap heap;
    int yaCalificadas = llenarHeap(heap, filtro);

    if (heap.vacio()) {
        if (filtro.empty())
            return "No hay peliculas nuevas para recomendar.";
        return "No hay peliculas nuevas del genero \"" + genero + "\".";
    }

    ostringstream o;
    o << fixed << setprecision(1);

    int insertados = heap.tamanio();
    int intercambiosInsercion = heap.intercambios();
    if (devActivo()) {
        o << "[DEV] Catalogo: " << totalPeliculas() << " peliculas, "
          << yaCalificadas << " descartadas por ya calificadas\n";
        o << "[DEV] Max-Heap: " << insertados << " candidatos insertados, "
          << intercambiosInsercion << " intercambios al flotar\n";
        if (insertados <= 20)
            o << "[DEV] arreglo del heap (id:puntaje): " << heap.estado() << "\n";
    }

    int k = n < insertados ? n : insertados;
    delete[] ultimaLista;
    ultimaLista = new Candidato[k];
    tamUltimaLista = k;
    for (int i = 0; i < k; i++)
        ultimaLista[i] = heap.extraerMax();

    if (devActivo()) {
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
        const Pelicula* p = obtenerPelicula(ultimaLista[i].id);
        o << "\n  " << (i + 1) << ". [" << ultimaLista[i].id << "] ";
        if (p != nullptr)
            o << p->titulo << " (" << p->anio << ") - " << p->genero;
        o << " - puntaje " << ultimaLista[i].puntaje;
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
    const Pelicula* p = obtenerPelicula(id);
    if (p == nullptr) {
        ostringstream e;
        e << "ERROR: la pelicula " << id << " ya no esta en el catalogo.";
        return e.str();
    }

    double afinidad = 2.0 * c.compGenero - 1.0;

    ostringstream o;
    o << fixed << setprecision(1);
    o << "[" << p->id << "] " << p->titulo << " - puesto " << (pos + 1)
      << " de " << tamUltimaLista << "\n";

    if (c.usadasGenero == 0)
        o << "  Genero " << p->genero << ": no calificaste ninguna, se toma neutral\n";
    else
        o << "  Genero " << p->genero << ": afinidad " << showpos << setprecision(2)
          << afinidad << noshowpos << setprecision(1) << " (segun " << c.usadasGenero
          << " calificacion" << (c.usadasGenero == 1 ? "" : "es") << " tuya"
          << (c.usadasGenero == 1 ? "" : "s") << ")\n";

    o << "  Componente genero       : " << setprecision(2) << c.compGenero << " x "
      << (int)(PESO_GENERO * 100 + 0.5) << "% = " << setprecision(1)
      << 100.0 * PESO_GENERO * c.compGenero << " pts\n";
    o << "  Componente calidad      : " << setprecision(2) << c.compCalidad * 5.0
      << "/5 ajustado (" << p->numVotos << " votos) x "
      << (int)(PESO_CALIDAD * 100 + 0.5) << "% = " << setprecision(1)
      << 100.0 * PESO_CALIDAD * c.compCalidad << " pts\n";
    o << "  Componente popularidad  : " << setprecision(2) << c.compPopularidad << " x "
      << (int)(PESO_POPULARIDAD * 100 + 0.5) << "% = " << setprecision(1)
      << 100.0 * PESO_POPULARIDAD * c.compPopularidad << " pts\n";
    o << "  Componente colaborativo : " << setprecision(2) << c.compColaborativo << " x "
      << (int)(PESO_COLABORATIVO * 100 + 0.5) << "% = " << setprecision(1)
      << 100.0 * PESO_COLABORATIVO * c.compColaborativo << " pts\n";
    o << "  Puntaje total           : " << c.puntaje << " / 100";
    return o.str();
}

int topN(int n, int* idsDestino) {
    if (n <= 0 || idsDestino == nullptr || totalPeliculas() <= 0)
        return 0;

    MaxHeap heap;
    llenarHeap(heap, "");
    int k = 0;
    while (k < n && !heap.vacio())
        idsDestino[k++] = heap.extraerMax().id;
    return k;
}
