#ifndef MODULOS_H
#define MODULOS_H

// CONTRATO ENTRE EL INTERPRETE (Integrante 5) Y LOS MODULOS DEL GRUPO.
// Cada integrante implementa sus funciones EXACTAMENTE con estas firmas
// e incluye este archivo en su .cpp para que el compilador verifique.
// Todas devuelven el texto que se mostrara al usuario.

#include <string>

// ---- Integrante 1: Bibliotecario (Hash + AVL) ----
bool cargarCatalogo(const std::string& ruta);      // llamada una vez al iniciar
std::string buscarPorId(int id);
std::string buscarPorGenero(const std::string& genero);
std::string buscarPorAnio(int desde, int hasta);

// ---- Integrante 2: Gestor de Perfiles (lista doble, pila, cola) ----
std::string calificar(int id, int estrellas);
std::string deshacer();
std::string verHistorial();
std::string descubrir();
std::string siguiente();

// ---- Integrante 3: Matematico (Max-Heap) ----
// genero vacio ("") significa "sin filtro de genero".
// recomendar() guarda internamente la ultima lista para que explicar() funcione.
// Si el id no esta en esa ultima lista, explicar() devuelve un mensaje de error.
std::string recomendar(int n, const std::string& genero);
std::string explicar(int id);

// ---- Integrante 4: Analista Social (Grafos + Jaccard) ----
std::string afinidad(int usuario);

#endif