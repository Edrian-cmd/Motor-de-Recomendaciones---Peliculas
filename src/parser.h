#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <string>
#include <vector>
#include "token.h"
#include "ast.h"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    Comando* parsear();                        // nullptr si hay error
    const std::string& mensajeError() const;

private:
    std::vector<Token> tokens;
    std::size_t pos;
    std::string error;

    const Token& actual() const;
    void fallar(const std::string& esperado);
    bool esperar(TipoToken tipo, const std::string& descripcion);
    bool leerEntero(int& valor);
    bool leerCadena(std::string& texto);
    Comando* nuevo(TipoComando tipo);

    Comando* parsearBuscar();
    Comando* parsearCalificar();
    Comando* parsearRecomendar();
    Comando* parsearExplicar();
    Comando* parsearAfinidad();
    Comando* parsearDev();
};

#endif