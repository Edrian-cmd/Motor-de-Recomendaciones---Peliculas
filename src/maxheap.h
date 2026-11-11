#ifndef MAXHEAP_H
#define MAXHEAP_H

// Integrante 3: Matematico (Motor de Recomendacion Personalizado)
// Monticulo de maximos sobre un arreglo dinamico (new/delete).
// Sirve para sacar el Top-N sin ordenar todo el catalogo:
//   insertar  -> O(log m)
//   extraerMax-> O(log m)
// Top-N completo: O(m log m + N log m), sin Bubble Sort ni QuickSort.

#include <string>

struct Candidato {
    int id;
    double puntaje;         // 0 a 100
    double compGenero;      // 0 a 1, afinidad del usuario con el genero
    double compCalidad;     // 0 a 1, rating promedio / 5
};

class MaxHeap {
public:
    explicit MaxHeap(int capacidadInicial = 16);
    ~MaxHeap();

    void insertar(const Candidato& c);
    Candidato extraerMax();             // precondicion: !vacio()
    bool vacio() const;
    int tamanio() const;

    // Para el modo desarrollador
    int intercambios() const;
    std::string estado() const;         // arreglo interno en orden de niveles

private:
    Candidato* datos;
    int cantidad;
    int capacidad;
    int numIntercambios;

    void crecer();
    void flotar(int i);                 // sift-up
    void hundir(int i);                 // sift-down
    void intercambiar(int i, int j);
    static bool mayor(const Candidato& a, const Candidato& b);

    // No copiable: el heap es duenio de su arreglo
    MaxHeap(const MaxHeap&);
    MaxHeap& operator=(const MaxHeap&);
};

#endif
