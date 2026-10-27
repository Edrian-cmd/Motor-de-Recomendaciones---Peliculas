#include "modulos.h"
#include "avl.h"
#include "hash.h"

using namespace std;

// Instancia global de tu modulo independiente (Caja Negra)
TablaHash miCatalogoHash;
ArbolAVL miCatalogoAVL;

bool cargarCatalogo(const string& ruta) {
    (void)ruta; // Evitar advertencia de variable no utilizada
    
    // MOCKS: Inyeccion manual de datos para probar la estructura
    miCatalogoHash.insertar(101, "Matrix", "Ciencia Ficcion", 1999);
    miCatalogoHash.insertar(102, "Titanic", "Romance", 1997);
    miCatalogoHash.insertar(103, "Shrek", "Animacion", 2001);
    
    return true; 
}

string buscarPorId(int id) {
    // Conectando el contrato estricto con tu modulo Hash
    return miCatalogoHash.buscar(id);
}

string buscarPorGenero(const string& genero) {
    return miCatalogoAVL.buscarPorGenero(genero);
}

string buscarPorAnio(int desde, int hasta) {
    return miCatalogoAVL.buscarPorRango(desde, hasta);
}