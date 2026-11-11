#include "modulos.h"
#include "avl.h"
#include "hash.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

TablaHash miCatalogoHash;
ArbolAVL miCatalogoAVL;

bool cargarCatalogo(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo en la ruta: " << ruta << "\n";
        return false;
    }

    string linea;
    int peliculasCargadas = 0;

    // Leer el archivo linea por linea
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string id_str, titulo, genero, anio_str;

        // Separar los datos por coma (ajusta a ';' si tu archivo usa punto y coma)
        getline(ss, id_str, ',');
        getline(ss, titulo, ',');
        getline(ss, genero, ',');
        getline(ss, anio_str, ',');

        try {
            int id = stoi(id_str);
            int anio = stoi(anio_str);

            // Insercion en las dos estructuras de datos
            miCatalogoHash.insertar(id, titulo, genero, anio);
            miCatalogoAVL.insertar(id, titulo, genero, anio); 

            peliculasCargadas++;
        } catch (...) {
            // Ignora silenciosamente la primera linea si es el encabezado de las columnas
        }
    }

    archivo.close();
    cout << "Carga completada: " << peliculasCargadas << " peliculas registradas en la Tabla Hash y Arbol AVL.\n";
    return true;
}

string buscarPorId(int id) {
    return miCatalogoHash.buscar(id);
}

string buscarPorGenero(const string& genero) {
    return "Resultado desde Arbol AVL: Filtrando genero " + genero;
}