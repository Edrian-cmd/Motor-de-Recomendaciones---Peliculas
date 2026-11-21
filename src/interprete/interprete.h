#ifndef INTERPRETE_H
#define INTERPRETE_H

#include <string>
#include "ast.h"

class Interprete {
public:
    Interprete();
    void ejecutar();                    // bucle principal

private:
    bool modoDev;
    std::string despachar(const Comando& cmd);
    std::string textoAyuda() const;
};

#endif