#include "token.h"

using namespace std; 

string nombreToken(TipoToken t) {
    switch (t) {
        case TipoToken::BUSCAR:      return "BUSCAR";
        case TipoToken::CALIFICAR:   return "CALIFICAR";
        case TipoToken::RECOMENDAR:  return "RECOMENDAR";
        case TipoToken::EXPLICAR:    return "EXPLICAR";
        case TipoToken::DESHACER:    return "DESHACER";
        case TipoToken::HISTORIAL:   return "HISTORIAL";
        case TipoToken::DESCUBRIR:   return "DESCUBRIR";
        case TipoToken::SIGUIENTE:   return "SIGUIENTE";
        case TipoToken::AFINIDAD:    return "AFINIDAD";
        case TipoToken::DEV:         return "DEV";
        case TipoToken::AYUDA:       return "AYUDA";
        case TipoToken::SALIR:       return "SALIR";
        case TipoToken::ID:          return "ID";
        case TipoToken::GENERO:      return "GENERO";
        case TipoToken::ANIO:        return "ANIO";
        case TipoToken::TOP:         return "TOP";
        case TipoToken::USUARIO:     return "USUARIO";
        case TipoToken::ON:          return "ON";
        case TipoToken::OFF:         return "OFF";
        case TipoToken::ENTERO:      return "ENTERO";
        case TipoToken::CADENA:      return "CADENA";
        case TipoToken::PUNTOS:      return "PUNTOS";
        case TipoToken::FIN:         return "FIN";
        case TipoToken::DESCONOCIDO: return "DESCONOCIDO";
    }
    return "?";
}