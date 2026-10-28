#include "interprete.h"
#include "lexer.h"
#include "parser.h"
#include "modulos.h"
#include "recomendador.h"
#include "afinidad.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>

using namespace std;

static string rutaCatalogo() {
    const char* candidatas[] = { "data/peliculas.txt", "../data/peliculas.txt", "peliculas.txt" };
    for (int i = 0; i < 3; i++) {
        ifstream f(candidatas[i]);
        if (f.is_open()) return candidatas[i];
    }
    return "data/peliculas.txt";
}

Interprete::Interprete() : modoDev(false) {}

string Interprete::textoAyuda() const {
    return
        "Comandos disponibles:\n"
        "  buscar id <n>              buscar una pelicula por id\n"
        "  buscar genero \"<texto>\"    buscar por genero\n"
        "  buscar anio <a>[..<b>]     buscar por anio o rango\n"
        "  calificar id <n> <1-5>     calificar una pelicula\n"
        "  recomendar top <n> [genero \"<texto>\"]\n"
        "  explicar id <n>            explicar una recomendacion\n"
        "  deshacer                   deshacer la ultima accion\n"
        "  historial                  ver el historial\n"
        "  descubrir / siguiente      modo descubrimiento rapido\n"
        "  afinidad usuario <n>       usuarios afines\n"
        "  dev on | dev off           modo desarrollador\n"
        "  ayuda                      mostrar esta ayuda\n"
        "  salir                      terminar";
}

string Interprete::despachar(const Comando& cmd) {
    switch (cmd.tipo) {
        case TipoComando::BUSCAR_ID:     return buscarPorId(cmd.id);
        case TipoComando::BUSCAR_GENERO: return buscarPorGenero(cmd.genero);
        case TipoComando::BUSCAR_ANIO:   return buscarPorAnio(cmd.anioDesde, cmd.anioHasta);
        case TipoComando::CALIFICAR:     return calificar(cmd.id, cmd.calificacion);
        case TipoComando::RECOMENDAR:    return recomendar(cmd.n, cmd.genero);
        case TipoComando::EXPLICAR:      return explicar(cmd.id);
        case TipoComando::DESHACER:      return deshacer();
        case TipoComando::HISTORIAL:     return verHistorial();
        case TipoComando::DESCUBRIR:     return descubrir();
        case TipoComando::SIGUIENTE:     return siguiente();
        case TipoComando::AFINIDAD:      return afinidad(cmd.usuario);
        case TipoComando::DEV_ON:
            modoDev = true;
            recomendadorModoDev(true);
            afinidadModoDev(true);
            return "Modo desarrollador activado";
        case TipoComando::DEV_OFF:
            modoDev = false;
            recomendadorModoDev(false);
            afinidadModoDev(false);
            return "Modo desarrollador desactivado";
        case TipoComando::AYUDA:         return textoAyuda();
        case TipoComando::SALIR:         return "";
    }
    return "";
}

void Interprete::ejecutar() {
    cout << "=== MOTOR DE RECOMENDACIONES DE PELICULAS - GRUPO 5 ===\n";
    cout << "Escribe 'ayuda' para ver los comandos.\n";
    cout << "Ejemplos: buscar id 104 | recomendar top 3 | afinidad usuario 1 | ayuda\n";

        if (!cargarCatalogo(rutaCatalogo()))
        cout << "Aviso: no se pudo cargar el catalogo.\n";

    string linea;
    while (true) {
        cout << "> ";
        if (!getline(cin, linea)) break;

        Lexer lexer(linea);
        vector<Token> tokens = lexer.tokenizar();

        if (tokens.size() == 1)             // solo FIN: linea vacia
            continue;

        if (modoDev) {
            cout << "[DEV] tokens:";
            for (size_t i = 0; i < tokens.size(); i++)
                cout << " " << nombreToken(tokens[i].tipo);
            cout << "\n";
        }

        Parser parser(tokens);
        Comando* cmd = parser.parsear();

        if (cmd == nullptr) {
            cout << "ERROR: " << parser.mensajeError() << "\n";
            continue;
        }
        if (cmd->tipo == TipoComando::SALIR) {
            delete cmd;
            break;
        }

        chrono::steady_clock::time_point t0 = chrono::steady_clock::now();
        string resultado = despachar(*cmd);
        chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

        if (modoDev) {
            double ms = chrono::duration<double, milli>(t1 - t0).count();
            cout << "[DEV] comando: " << nombreComando(cmd->tipo) << "\n";
            cout << "[DEV] tiempo: " << ms << " ms\n";
        }

        cout << resultado << "\n";
        delete cmd;
    }
    cout << "Hasta luego.\n";
}