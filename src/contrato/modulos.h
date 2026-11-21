#ifndef MODULOS_H
#define MODULOS_H

// =====================================================================
// CONTRATO v2 entre los modulos del Motor de Recomendaciones.
// Cada funcion la implementa UN solo integrante; el resto solo la llama.
// Las funciones que devuelven std::string son las que usa el interprete
// para responder comandos (no imprimen con cout); las demas son el
// "pegamento" entre modulos. Ver docs/CONTEXTO_PROYECTO.md, seccion 4.
// =====================================================================

#include <string>
#include "modelo.h"

// ---- Integrante 1: Bibliotecario (Hash + AVL) -> catalogo/ ----
bool cargarCatalogo(const std::string& ruta);
std::string buscarPorId(int id);
std::string buscarPorGenero(const std::string& genero);
std::string buscarPorAnio(int desde, int hasta);
const Pelicula* obtenerPelicula(int id);           // nullptr si no existe, O(1) promedio
int totalPeliculas();                              // O(1)
const Pelicula* peliculaPorIndice(int i);          // 0..total-1, O(1)
void registrarVoto(int idPelicula, int estrellasNuevas, int estrellasAnteriores);
// Comando "comparar" (PR 6): tiempos de busqueda lineal vs Hash vs AVL
std::string compararPorId(int id);
std::string compararPorAnio(int desde, int hasta);

// ---- Integrante 2: Gestor de Perfiles (lista doble, pila, cola) -> perfiles/ ----
// (perfilCargarCalificacion y perfilCambiarUsuario estan en Gestor_Perfil.h)
std::string calificar(int id, int estrellas);
std::string deshacer();
std::string verHistorial();
std::string descubrir();
std::string siguiente();
int usuarioActivo();
int estrellasDelActivo(int idPelicula);            // 0 = no la califico
int calificacionesDelActivo(Calificacion* destino, int max);

// ---- Integrante 3: Matematico (Max-Heap) -> recomendador/ ----
std::string recomendar(int n, const std::string& genero);
std::string explicar(int id);
int topN(int n, int* idsDestino);

// ---- Integrante 4: Analista Social (Grafo + Jaccard) -> afinidad/ ----
std::string afinidad(int usuario);
void grafoRegistrarCalificacion(int idUsuario, int idPelicula, int estrellas);
double puntajeColaborativo(int idPelicula);        // 0..1

// ---- Integrante 5: Interprete, persistencia, estadisticas ----
bool modoDevActivo();                              // interprete/
bool cargarDatos();                                // persistencia/
bool guardarDatos();                               // persistencia/
std::string cambiarUsuario(int idUsuario);         // persistencia/
std::string estadisticas();                        // estadisticas/

#endif
