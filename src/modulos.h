#ifndef MODULOS_H
#define MODULOS_H

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

std::string recomendar(int n, const std::string& genero);
std::string explicar(int id);

// ---- Integrante 4: Analista Social (Grafos + Jaccard) ----
std::string afinidad(int usuario);

#endif