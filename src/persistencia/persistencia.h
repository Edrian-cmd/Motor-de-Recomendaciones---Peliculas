#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

// Integrante 5 (Edrian): persistencia de usuarios y calificaciones.
// cargarDatos(), guardarDatos() y cambiarUsuario() estan en modulos.h.
// Aqui va lo propio del modulo, que usa el interprete para sus mensajes.

#include <string>

int totalUsuarios();                         // usuarios leidos de data/usuarios.txt (+ nuevos)
int totalCalificacionesCargadas();           // filas validas leidas al iniciar
std::string nombreUsuario(int idUsuario);    // "" si no existe

#endif
