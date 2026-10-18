#include "ast.h"

using namespace std;

string nombreComando(TipoComando t) {
    switch (t) {
        case TipoComando::BUSCAR_ID:     return "BUSCAR_ID";
        case TipoComando::BUSCAR_GENERO: return "BUSCAR_GENERO";
        case TipoComando::BUSCAR_ANIO:   return "BUSCAR_ANIO";
        case TipoComando::CALIFICAR:     return "CALIFICAR";
        case TipoComando::RECOMENDAR:    return "RECOMENDAR";
        case TipoComando::EXPLICAR:      return "EXPLICAR";
        case TipoComando::DESHACER:      return "DESHACER";
        case TipoComando::HISTORIAL:     return "HISTORIAL";
        case TipoComando::DESCUBRIR:     return "DESCUBRIR";
        case TipoComando::SIGUIENTE:     return "SIGUIENTE";
        case TipoComando::AFINIDAD:      return "AFINIDAD";
        case TipoComando::DEV_ON:        return "DEV_ON";
        case TipoComando::DEV_OFF:       return "DEV_OFF";
        case TipoComando::AYUDA:         return "AYUDA";
        case TipoComando::SALIR:         return "SALIR";
    }
    return "?";
}