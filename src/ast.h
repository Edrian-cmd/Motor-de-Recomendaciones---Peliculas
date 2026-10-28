#ifndef AST_H
#define AST_H

#include <string>

enum class TipoComando {
    BUSCAR_ID, BUSCAR_GENERO, BUSCAR_ANIO,
    CALIFICAR, RECOMENDAR, EXPLICAR, DESHACER,
    HISTORIAL, DESCUBRIR, SIGUIENTE, AFINIDAD,
    DEV_ON, DEV_OFF, AYUDA, SALIR
};

struct Comando {
    TipoComando tipo;
    int id = 0;
    int calificacion = 0;
    int n = 0;
    int usuario = 0;
    int anioDesde = 0;
    int anioHasta = 0;
    std::string genero;     // vacio si no se indico
};

std::string nombreComando(TipoComando t);

#endif