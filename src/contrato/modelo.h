#ifndef MODELO_H
#define MODELO_H

// =====================================================================
// Modelo de datos compartido por todos los modulos (contrato v2).
// Solo estructuras simples: cada modulo decide como almacenarlas.
// =====================================================================

#include <string>

// Una pelicula del catalogo. La crea y es duenio el Bibliotecario (Int.1,
// Tabla Hash); los demas modulos solo la leen mediante const Pelicula*.
// El orden de los campos importa: los tests la inicializan como
//   {id, titulo, genero, anio, ratingPromedio, numVotos}
struct Pelicula {
    int id;
    std::string titulo;
    std::string genero;
    int anio;
    double ratingPromedio;   // promedio de estrellas de TODOS los usuarios (0 si no hay votos)
    int numVotos;            // cuantos usuarios la calificaron
};

// Una calificacion de un usuario a una pelicula.
// Inicializacion: {idUsuario, idPelicula, estrellas}
// Convencion del proyecto: estrellas == 0 significa "sin calificar / eliminada".
struct Calificacion {
    int idUsuario;
    int idPelicula;
    int estrellas;           // 1 a 5
};

#endif
