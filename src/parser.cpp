#include "parser.h"
#include <sstream>

using namespace std;

Parser::Parser(const vector<Token>& tokens) : tokens(tokens), pos(0) {}

const string& Parser::mensajeError() const { return error; }

const Token& Parser::actual() const { return tokens[pos]; }

// Registra solo el PRIMER error
void Parser::fallar(const string& esperado) {
    if (!error.empty()) return;
    const Token& t = actual();
    ostringstream oss;
    oss << "columna " << t.columna << ": se esperaba " << esperado
        << " pero se encontro ";
    if (t.tipo == TipoToken::FIN) oss << "el fin de la linea";
    else oss << "'" << t.lexema << "'";
    error = oss.str();
}

bool Parser::esperar(TipoToken tipo, const string& descripcion) {
    if (actual().tipo != tipo) {
        fallar(descripcion);
        return false;
    }
    pos++;
    return true;
}

bool Parser::leerEntero(int& valor) {
    if (actual().tipo != TipoToken::ENTERO) {
        fallar("un numero entero");
        return false;
    }
    valor = actual().valor;
    pos++;
    return true;
}

bool Parser::leerCadena(string& texto) {
    if (actual().tipo != TipoToken::CADENA) {
        fallar("un texto entre comillas");
        return false;
    }
    texto = actual().lexema;
    pos++;
    return true;
}

Comando* Parser::nuevo(TipoComando tipo) {
    Comando* c = new Comando();
    c->tipo = tipo;
    return c;
}

Comando* Parser::parsear() {
    Comando* cmd = nullptr;

    switch (actual().tipo) {
        case TipoToken::BUSCAR:     pos++; cmd = parsearBuscar();     break;
        case TipoToken::CALIFICAR:  pos++; cmd = parsearCalificar();  break;
        case TipoToken::RECOMENDAR: pos++; cmd = parsearRecomendar(); break;
        case TipoToken::EXPLICAR:   pos++; cmd = parsearExplicar();   break;
        case TipoToken::AFINIDAD:   pos++; cmd = parsearAfinidad();   break;
        case TipoToken::DEV:        pos++; cmd = parsearDev();        break;
        case TipoToken::DESHACER:   pos++; cmd = nuevo(TipoComando::DESHACER);  break;
        case TipoToken::HISTORIAL:  pos++; cmd = nuevo(TipoComando::HISTORIAL); break;
        case TipoToken::DESCUBRIR:  pos++; cmd = nuevo(TipoComando::DESCUBRIR); break;
        case TipoToken::SIGUIENTE:  pos++; cmd = nuevo(TipoComando::SIGUIENTE); break;
        case TipoToken::AYUDA:      pos++; cmd = nuevo(TipoComando::AYUDA);     break;
        case TipoToken::SALIR:      pos++; cmd = nuevo(TipoComando::SALIR);     break;
        default:
            fallar("un comando (buscar, calificar, recomendar, ...)");
            return nullptr;
    }

    if (cmd == nullptr) return nullptr;

    // Despues de un comando completo solo puede venir el fin de linea
    if (actual().tipo != TipoToken::FIN) {
        fallar("el fin de la linea");
        delete cmd;
        return nullptr;
    }
    return cmd;
}

Comando* Parser::parsearBuscar() {
    switch (actual().tipo) {
        case TipoToken::ID: {
            pos++;
            int id;
            if (!leerEntero(id)) return nullptr;
            Comando* c = nuevo(TipoComando::BUSCAR_ID);
            c->id = id;
            return c;
        }
        case TipoToken::GENERO: {
            pos++;
            string g;
            if (!leerCadena(g)) return nullptr;
            Comando* c = nuevo(TipoComando::BUSCAR_GENERO);
            c->genero = g;
            return c;
        }
        case TipoToken::ANIO: {
            pos++;
            int desde, hasta;
            if (!leerEntero(desde)) return nullptr;
            hasta = desde;
            if (actual().tipo == TipoToken::PUNTOS) {      // parte opcional: ..ENTERO
                pos++;
                if (!leerEntero(hasta)) return nullptr;
            }
            Comando* c = nuevo(TipoComando::BUSCAR_ANIO);
            c->anioDesde = desde;
            c->anioHasta = hasta;
            return c;
        }
        default:
            fallar("'id', 'genero' o 'anio'");
            return nullptr;
    }
}

Comando* Parser::parsearCalificar() {
    int id, calificacion;
    if (!esperar(TipoToken::ID, "'id'")) return nullptr;
    if (!leerEntero(id)) return nullptr;
    if (!leerEntero(calificacion)) return nullptr;
    Comando* c = nuevo(TipoComando::CALIFICAR);
    c->id = id;
    c->calificacion = calificacion;
    return c;
}

Comando* Parser::parsearRecomendar() {
    int n;
    string genero;
    if (!esperar(TipoToken::TOP, "'top'")) return nullptr;
    if (!leerEntero(n)) return nullptr;
    if (actual().tipo == TipoToken::GENERO) {              // parte opcional
        pos++;
        if (!leerCadena(genero)) return nullptr;
    }
    Comando* c = nuevo(TipoComando::RECOMENDAR);
    c->n = n;
    c->genero = genero;
    return c;
}

Comando* Parser::parsearExplicar() {
    int id;
    if (!esperar(TipoToken::ID, "'id'")) return nullptr;
    if (!leerEntero(id)) return nullptr;
    Comando* c = nuevo(TipoComando::EXPLICAR);
    c->id = id;
    return c;
}

Comando* Parser::parsearAfinidad() {
    int usuario;
    if (!esperar(TipoToken::USUARIO, "'usuario'")) return nullptr;
    if (!leerEntero(usuario)) return nullptr;
    Comando* c = nuevo(TipoComando::AFINIDAD);
    c->usuario = usuario;
    return c;
}

Comando* Parser::parsearDev() {
    if (actual().tipo == TipoToken::ON)  { pos++; return nuevo(TipoComando::DEV_ON); }
    if (actual().tipo == TipoToken::OFF) { pos++; return nuevo(TipoComando::DEV_OFF); }
    fallar("'on' u 'off'");
    return nullptr;
}