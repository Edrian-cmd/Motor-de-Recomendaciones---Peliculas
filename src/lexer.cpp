#include "lexer.h"
#include <unordered_map>
#include <cctype>
#include <cstdlib>

using namespace std; 

enum class Estado { INICIO, PALABRA, NUMERO, CADENA, PUNTO }; 

static bool esLetra(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); 
}

static bool esDigito(char c){
    return c >= '0' && c <= '9'; 
}

static string aMinuscula(string s){
    for (size_t i = 0; i< s.size(); i++)
        s[i] = (char)tolower((unsigned char)s[i]); 
    return s; 
}

static Token crear(TipoToken tipo, const string& lexema, int valor, int columna){
    Token t; 
    t.tipo = tipo; 
    t.lexema = lexema; 
    t.valor = valor; 
    t.columna = columna; 
    return t; 
}

// Tabla hash de palabras reservadas: texto -> tipo de token
static const unordered_map<string, TipoToken>& palabrasReservadas() {
    static const unordered_map<string, TipoToken> tabla = {
        {"buscar", TipoToken::BUSCAR},       {"calificar", TipoToken::CALIFICAR},
        {"recomendar", TipoToken::RECOMENDAR}, {"explicar", TipoToken::EXPLICAR},
        {"deshacer", TipoToken::DESHACER},   {"historial", TipoToken::HISTORIAL},
        {"descubrir", TipoToken::DESCUBRIR}, {"siguiente", TipoToken::SIGUIENTE},
        {"afinidad", TipoToken::AFINIDAD},   {"dev", TipoToken::DEV},
        {"ayuda", TipoToken::AYUDA},         {"salir", TipoToken::SALIR},
        {"id", TipoToken::ID},               {"genero", TipoToken::GENERO},
        {"anio", TipoToken::ANIO},           {"top", TipoToken::TOP},
        {"usuario", TipoToken::USUARIO},     {"on", TipoToken::ON},
        {"off", TipoToken::OFF}
    };
    return tabla;
}

static Token clasificarPalabra(const string& texto, int columna){
    const unordered_map<string, TipoToken>& tabla = palabrasReservadas(); 
    unordered_map<string, TipoToken>::const_iterator it = tabla.find(aMinuscula(texto)); 
    if(it != tabla.end())
        return crear(it->second, texto, 0, columna);
    return crear(TipoToken::DESCONOCIDO, texto, 0, columna);  
}   

static Token clasificarNumero(const string& texto, int columna){
    // Mas de 9 dígitos desbordaria un int
    if (texto.size()>9)
        return crear(TipoToken::DESCONOCIDO, texto, 0, columna); 
    return crear(TipoToken::ENTERO, texto, atoi(texto.c_str()), columna); 
}

Lexer::Lexer(const  string& linea) : linea(linea) {}

vector<Token> Lexer::tokenizar() {
    vector<Token> tokens;
    Estado estado = Estado::INICIO;
    string acumulado;
    int inicio = 0;
    size_t i = 0;
    bool terminado = false;

    while (!terminado) {
        // Al pasar el final de la línea, c vale '\0' (centinela de fin de entrada)
        char c = (i < linea.size()) ? linea[i] : '\0';

        switch (estado) {

        case Estado::INICIO:
            inicio = (int)i + 1;                    // columnas desde 1
            if (c == '\0') {
                terminado = true;
            } else if (c == ' ' || c == '\t') {
                i++;
            } else if (esLetra(c)) {
                acumulado = string(1, c);
                estado = Estado::PALABRA;
                i++;
            } else if (esDigito(c)) {
                acumulado = string(1, c);
                estado = Estado::NUMERO;
                i++;
            } else if (c == '"') {
                acumulado = "";
                estado = Estado::CADENA;
                i++;
            } else if (c == '.') {
                estado = Estado::PUNTO;
                i++;
            } else {
                tokens.push_back(crear(TipoToken::DESCONOCIDO, string(1, c), 0, inicio));
                i++;
            }
            break;

        case Estado::PALABRA:
            if (esLetra(c) || esDigito(c)) {
                acumulado += c;
                i++;
            } else {
                tokens.push_back(clasificarPalabra(acumulado, inicio));
                estado = Estado::INICIO;            // sin i++: c se reprocesa
            }
            break;

        case Estado::NUMERO:
            if (esDigito(c)) {
                acumulado += c;
                i++;
            } else {
                tokens.push_back(clasificarNumero(acumulado, inicio));
                estado = Estado::INICIO;            // sin i++
            }
            break;

        case Estado::CADENA:
            if (c == '"') {
                tokens.push_back(crear(TipoToken::CADENA, acumulado, 0, inicio));
                estado = Estado::INICIO;
                i++;
            } else if (c == '\0') {
                // Llegó el fin sin cerrar las comillas
                tokens.push_back(crear(TipoToken::DESCONOCIDO, "\"" + acumulado, 0, inicio));
                estado = Estado::INICIO;            // sin i++
            } else {
                acumulado += c;
                i++;
            }
            break;

        case Estado::PUNTO:
            if (c == '.') {
                tokens.push_back(crear(TipoToken::PUNTOS, "..", 0, inicio));
                i++;
            } else {
                tokens.push_back(crear(TipoToken::DESCONOCIDO, ".", 0, inicio));
            }
            estado = Estado::INICIO;
            break;
        }
    }

    tokens.push_back(crear(TipoToken::FIN, "", 0, (int)linea.size() + 1));
    return tokens;
}