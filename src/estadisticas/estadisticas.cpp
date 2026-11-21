#include "modulos.h"
#include "maxheap.h"
#include "persistencia.h"
#include <sstream>
#include <iomanip>

using namespace std;

// =====================================================================
// Integrante 5 (Edrian): comando "estadisticas" (PR 6 del plan)
//   - total de peliculas por genero
//   - las peliculas mas populares (mas votos), usando el Max-Heap del Int.3
//     en lugar de ordenar el catalogo (sin Bubble Sort ni QuickSort)
//   - del usuario activo: cuantas califico, su promedio y su genero favorito
// Solo lee datos a traves del contrato (modulos.h); no imprime, devuelve string.
// =====================================================================

static const int TOP_POPULARES = 5;

// Conteo por genero en un arreglo dinamico. Hay ~13 generos, por eso la
// busqueda lineal en el arreglo (O(g)) es suficiente y facil de explicar.
struct ConteoGenero {
    string genero;
    int peliculas;        // peliculas del catalogo en este genero
    int calificadas;      // calificaciones del usuario activo en este genero
    int sumaEstrellas;    // suma de esas calificaciones
};

class TablaGeneros {
public:
    TablaGeneros() : datos(nullptr), cantidad(0), capacidad(0) {}
    ~TablaGeneros() { delete[] datos; }

    ConteoGenero& obtener(const string& genero) {
        for (int i = 0; i < cantidad; i++)
            if (datos[i].genero == genero) return datos[i];
        if (cantidad == capacidad) crecer();
        ConteoGenero& g = datos[cantidad++];
        g.genero = genero;
        g.peliculas = 0;
        g.calificadas = 0;
        g.sumaEstrellas = 0;
        return g;
    }
    int total() const { return cantidad; }
    const ConteoGenero& en(int i) const { return datos[i]; }

private:
    ConteoGenero* datos;
    int cantidad;
    int capacidad;

    // Duplica el arreglo cuando se llena: insercion amortizada O(1)
    void crecer() {
        int nueva = capacidad == 0 ? 8 : capacidad * 2;
        ConteoGenero* arr = new ConteoGenero[nueva];
        for (int i = 0; i < cantidad; i++) arr[i] = datos[i];
        delete[] datos;
        datos = arr;
        capacidad = nueva;
    }

    TablaGeneros(const TablaGeneros&);
    TablaGeneros& operator=(const TablaGeneros&);
};

string estadisticas() {
    int m = totalPeliculas();
    if (m <= 0)
        return "ERROR: el catalogo esta vacio.";

    TablaGeneros generos;
    MaxHeap populares;
    long votosTotales = 0;

    // Una pasada por el catalogo: O(m * g) para generos + O(m log m) para el heap
    for (int i = 0; i < m; i++) {
        const Pelicula* p = peliculaPorIndice(i);
        if (p == nullptr) continue;
        generos.obtener(p->genero).peliculas++;
        votosTotales += p->numVotos;
        if (p->numVotos > 0) {
            Candidato c;
            c.id = p->id;
            c.puntaje = p->numVotos;        // el heap ordena por votos
            c.compGenero = c.compCalidad = c.compPopularidad = c.compColaborativo = 0;
            c.usadasGenero = 0;
            populares.insertar(c);
        }
    }

    // Calificaciones del usuario activo (arreglo que crece si no alcanza)
    int capacidad = 64;
    Calificacion* propias = nullptr;
    int n = 0;
    while (true) {
        propias = new Calificacion[capacidad];
        n = calificacionesDelActivo(propias, capacidad);
        if (n < capacidad) break;
        delete[] propias;
        capacidad *= 2;
    }
    int sumaPropia = 0;
    for (int i = 0; i < n; i++) {
        sumaPropia += propias[i].estrellas;
        const Pelicula* p = obtenerPelicula(propias[i].idPelicula);
        if (p == nullptr) continue;
        ConteoGenero& g = generos.obtener(p->genero);
        g.calificadas++;
        g.sumaEstrellas += propias[i].estrellas;
    }
    delete[] propias;

    ostringstream o;
    o << fixed << setprecision(2);
    o << "=== Estadisticas del catalogo ===\n";
    o << "Peliculas: " << m << " | Usuarios: " << totalUsuarios()
      << " | Votos registrados: " << votosTotales << "\n";
    o << "Peliculas por genero:\n";
    for (int i = 0; i < generos.total(); i++)
        o << "  " << setw(12) << left << generos.en(i).genero << right
          << setw(4) << generos.en(i).peliculas << "\n";

    o << "Mas populares (mas votos):\n";
    for (int k = 1; k <= TOP_POPULARES && !populares.vacio(); k++) {
        Candidato c = populares.extraerMax();
        const Pelicula* p = obtenerPelicula(c.id);
        if (p == nullptr) continue;
        o << "  " << k << ". [" << p->id << "] " << p->titulo << " - " << p->numVotos
          << " votos, promedio " << p->ratingPromedio << "\n";
    }

    o << "=== Usuario activo: " << usuarioActivo() << " - " << nombreUsuario(usuarioActivo()) << " ===\n";
    if (n == 0) {
        o << "Aun no calificaste ninguna pelicula.";
    } else {
        // Genero favorito: mayor promedio propio; en empate, el que mas califico
        int fav = -1;
        double mejor = -1;
        for (int i = 0; i < generos.total(); i++) {
            const ConteoGenero& g = generos.en(i);
            if (g.calificadas == 0) continue;
            double prom = (double)g.sumaEstrellas / g.calificadas;
            if (fav == -1 || prom > mejor ||
                (prom == mejor && g.calificadas > generos.en(fav).calificadas)) {
                fav = i;
                mejor = prom;
            }
        }
        o << "Calificaciones: " << n << " | Promedio: " << (double)sumaPropia / n << " estrellas\n";
        o << "Genero favorito: " << generos.en(fav).genero << " (promedio " << mejor
          << " en " << generos.en(fav).calificadas << " pelicula"
          << (generos.en(fav).calificadas == 1 ? "" : "s") << ")";
    }

    if (modoDevActivo())
        o << "\n[DEV] Estadisticas: 1 pasada O(m) por " << m << " peliculas, "
          << generos.total() << " generos en arreglo dinamico, Max-Heap con "
          << populares.intercambios() << " intercambios";
    return o.str();
}
