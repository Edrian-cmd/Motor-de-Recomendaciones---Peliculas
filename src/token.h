#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TipoToken {
    // Comandos
    BUSCAR, CALIFICAR, RECOMENDAR, EXPLICAR, DESHACER,
    HISTORIAL, DESCUBRIR, SIGUIENTE, AFINIDAD, DEV, AYUDA, SALIR,
    // Palabras auxiliares
    ID, GENERO, ANIO, TOP, USUARIO, ON, OFF,
    // Tokens con valor
    ENTERO, CADENA,
    // Símbolos
    PUNTOS,
    // Control
    FIN, DESCONOCIDO
};

struct Token {
    TipoToken tipo;
    std::string lexema;   // texto exacto que escribió el usuario
    int valor = 0;        // solo se usa en ENTERO
    int columna = 0;      // posición en la línea, para mensajes de error
};

std::string nombreToken(TipoToken t);

#endif