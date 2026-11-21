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

// Interruptor global del modo desarrollador (contrato v2: modoDevActivo).
// Lo consultan todos los modulos para agregar su diagnostico [DEV].
static bool modoDevGlobal = false;

bool modoDevActivo() {
    return modoDevGlobal;
}

// Busca un archivo de datos tanto si el programa se ejecuta desde la raiz del
// repositorio como desde src/ o junto al archivo.
static string rutaDatos(const string& archivo) {
    string candidatas[] = { "data/" + archivo, "../data/" + archivo, archivo };
    for (int i = 0; i < 3; i++) {
        ifstream f(candidatas[i].c_str());
        if (f.is_open()) return candidatas[i];
    }
    return "data/" + archivo;
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
        "  afinidad usuario <n>       afinidad (Jaccard) con otro usuario\n"
        "  usuario <n>                cambiar el usuario activo\n"
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
        case TipoComando::CAMBIAR_USUARIO: return cambiarUsuario(cmd.usuario);
        case TipoComando::DEV_ON:
            modoDev = true;
            modoDevGlobal = true;
            recomendadorModoDev(true);
            afinidadModoDev(true);
            return "Modo desarrollador activado";
        case TipoComando::DEV_OFF:
            modoDev = false;
            modoDevGlobal = false;
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
    cout << "Ejemplos: buscar id 104 | recomendar top 3 | afinidad usuario 2 | ayuda\n";

    if (!cargarCatalogo(rutaDatos("peliculas.txt"))) {
        cout << "Aviso: no se pudo cargar el catalogo (data/peliculas.txt).\n";
    } else {
        cout << "Catalogo: " << totalPeliculas() << " peliculas.";
        int filas = cargarCalificaciones(rutaDatos("calificaciones.txt"));
        if (filas < 0)
            cout << " Aviso: no se encontro data/calificaciones.txt (sin comunidad).\n";
        else
            cout << " Comunidad: " << grafoTotalUsuarios() << " usuarios, "
                 << filas << " calificaciones.\n";
    }
    cout << "Usuario activo: " << usuarioActivo()
         << " (cambia con 'usuario <n>')\n";

    string linea;
    while (true) {
        cout << "> ";
        if (!getline(cin, linea)) break;

        // Limpia el BOM UTF-8 (lo agregan algunas consolas/redirecciones de
        // Windows) y el '\r' final de archivos con fin de linea CRLF.
        if (linea.size() >= 3 && (unsigned char)linea[0] == 0xEF &&
            (unsigned char)linea[1] == 0xBB && (unsigned char)linea[2] == 0xBF)
            linea = linea.substr(3);
        if (!linea.empty() && linea[linea.size() - 1] == '\r')
            linea.erase(linea.size() - 1);

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

        while (!resultado.empty() && resultado[resultado.size() - 1] == '\n')
            resultado.erase(resultado.size() - 1);
        cout << resultado << "\n";
        delete cmd;
    }
    cout << "Hasta luego.\n";
}