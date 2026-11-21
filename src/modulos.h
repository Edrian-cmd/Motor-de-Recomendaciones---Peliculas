#ifndef MODULOS_H
#define MODULOS_H

// =====================================================================
// CONTRATO v2 entre los modulos del Motor de Recomendaciones.
// Cada funcion la implementa UN solo integrante; el resto solo la llama.
// Las funciones que devuelven std::string son las que usa el interprete
// para responder comandos; las demas son el "pegamento" entre modulos.
// =====================================================================

#include <string>
#include "modelo.h"

// ---- Integrante 1: Bibliotecario (Hash + AVL) -> bibliotecario.cpp ----
bool cargarCatalogo(const std::string& ruta);      // llamada una vez al iniciar
std::string buscarPorId(int id);
std::string buscarPorGenero(const std::string& genero);
std::string buscarPorAnio(int desde, int hasta);

const Pelicula* obtenerPelicula(int id);           // O(1) promedio, nullptr si no existe
int totalPeliculas();                              // O(1)
const Pelicula* peliculaPorIndice(int i);          // 0 .. totalPeliculas()-1, O(1)
// Actualiza ratingPromedio y numVotos. estrellasAnteriores == 0 -> voto nuevo;
// estrellasNuevas == 0 -> se retira el voto.
void registrarVoto(int idPelicula, int estrellasNuevas, int estrellasAnteriores);

// ---- Integrante 2: Gestor de Perfiles (lista doble, pila, cola) -> Gestor_Perfil.cpp ----
std::string calificar(int id, int estrellas);
std::string deshacer();
std::string verHistorial();
std::string descubrir();
std::string siguiente();

int usuarioActivo();
int estrellasDelActivo(int idPelicula);                       // 0 si no la califico
int calificacionesDelActivo(Calificacion* destino, int max);  // copia y devuelve cuantas
void perfilCargarCalificacion(int idPelicula, int estrellas); // carga sin pila de deshacer
void perfilCambiarUsuario(int idUsuario);                     // limpia el perfil y cambia el activo

// ---- Integrante 3: Matematico (Max-Heap) -> recomendador.cpp ----
std::string recomendar(int n, const std::string& genero);
std::string explicar(int id);
int topN(int n, int* idsDestino);                  // ids de las n mejores no calificadas

// ---- Integrante 4: Analista Social (Grafos + Jaccard) -> afinidad.cpp ----
std::string afinidad(int usuario);
// Registra/actualiza una calificacion en el grafo. estrellas == 0 la elimina.
void grafoRegistrarCalificacion(int idUsuario, int idPelicula, int estrellas);
double puntajeColaborativo(int idPelicula);        // [0, 1] respecto al usuario activo
int grafoEstrellasDe(int idUsuario, int idPelicula);                     // 0 si no la califico
int grafoCalificacionesDe(int idUsuario, Calificacion* destino, int max); // historial de un usuario
int grafoTotalUsuarios();

// ---- Integrante 5: Interprete -> interprete.cpp ----
bool modoDevActivo();                              // interruptor global "dev on/off"

// ---- Carga de calificaciones de la comunidad -> calificaciones.cpp ----
// Lee "idUsuario,idPelicula,estrellas" y reparte cada fila al Hash (votos),
// al Grafo (red social) y al perfil del usuario activo. Devuelve las filas validas
// o -1 si no se pudo abrir el archivo.
int cargarCalificaciones(const std::string& ruta);
// Cambia el usuario activo y recarga su historial desde lo cargado.
std::string cambiarUsuario(int idUsuario);

#endif
