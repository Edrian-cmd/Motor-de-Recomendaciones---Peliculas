#ifndef GESTOR_PERFIL_H
#define GESTOR_PERFIL_H

#include <string>
#include "modelo.h"

std::string calificar(int idPelicula, int estrellas);

std::string deshacer();

std::string verHistorial();

std::string descubrir();

std::string siguiente();

int usuarioActivo();

int estrellasDelActivo(int idPelicula);

int calificacionesDelActivo(Calificacion* destino, int max);

void perfilCargarCalificacion(int idPelicula, int estrellas);

void perfilCambiarUsuario(int idUsuario);

void limpiarMemoriaPerfiles();

#endif
