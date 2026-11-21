#include "interprete.h"
#include "lexer.h"
#include "parser.h"
#include "modulos.h"
#include "persistencia.h"
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

// Interruptor global del modo desarrollador (contrato v2: modoDevActivo).
// Lo consultan todos los modulos para agregar su diagnostico [DEV].
static bool modoDevGlobal = false;

bool modoDevActivo() {
    return modoDevGlobal;
}

Interprete::Interprete() : modoDev(false) {}

string Interprete::textoAyuda() const {
    return
        "Comandos disponibles (lo que va entre < > se reemplaza por un valor, sin los signos):\n"
        "  buscar id <n>              buscar una pelicula por id        ej: buscar id 13\n"
        "  buscar genero \"<texto>\"    buscar por genero                 ej: buscar genero \"Sci-Fi\"\n"
        "  buscar anio <a>[..<b>]     buscar por anio o rango           ej: buscar anio 1990..1999\n"
        "  calificar id <n> <1-5>     calificar una pelicula            ej: calificar id 13 5\n"
        "  recomendar top <n> [genero \"<texto>\"]                        ej: recomendar top 5\n"
        "  explicar id <n>            explicar una recomendacion        ej: explicar id 13\n"
        "  deshacer                   deshacer la ultima calificacion\n"
        "  historial                  ver tus calificaciones\n"
        "  descubrir / siguiente      modo descubrimiento rapido (cola)\n"
        "  afinidad usuario <n>       afinidad (Jaccard) con otro usuario ej: afinidad usuario 2\n"
        "  usuario <n>                cambiar el usuario activo         ej: usuario 4\n"
        "  estadisticas               resumen del catalogo y de tu perfil\n"
        "  comparar id <n>            tiempos: lineal vs Hash vs AVL    ej: comparar id 13\n"
        "  comparar anio <a>[..<b>]   tiempos: lineal vs AVL            ej: comparar anio 1990..1999\n"
        "  dev on | dev off           modo desarrollador\n"
        "  ayuda                      mostrar esta ayuda\n"
        "  salir                      guardar y terminar";
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
        case TipoComando::ESTADISTICAS:  return estadisticas();
        case TipoComando::COMPARAR_ID:   return compararPorId(cmd.id);
        case TipoComando::COMPARAR_ANIO: return compararPorAnio(cmd.anioDesde, cmd.anioHasta);
        case TipoComando::DEV_ON:
            modoDev = true;
            modoDevGlobal = true;
            return "Modo desarrollador activado";
        case TipoComando::DEV_OFF:
            modoDev = false;
            modoDevGlobal = false;
            return "Modo desarrollador desactivado";
        case TipoComando::AYUDA:         return textoAyuda();
        case TipoComando::SALIR:         return "";
    }
    return "";
}

void Interprete::ejecutar() {
    cout << "=== MOTOR DE RECOMENDACIONES DE PELICULAS - GRUPO 5 ===\n";
    cout << "Escribe 'ayuda' para ver los comandos.\n";
    cout << "Ejemplos: buscar id 13 | recomendar top 5 | afinidad usuario 2 | usuario 4\n";

    if (!cargarDatos()) {
        cout << "Aviso: no se pudo cargar el catalogo (data/peliculas.txt).\n";
    } else {
        cout << "Catalogo: " << totalPeliculas() << " peliculas | Usuarios: "
             << totalUsuarios() << " | Calificaciones: " << totalCalificacionesCargadas() << "\n";
    }
    cout << "Usuario activo: " << usuarioActivo() << " - " << nombreUsuario(usuarioActivo())
         << " (cambia con 'usuario <n>', por ejemplo: usuario 4)\n";

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
            // Error comun: copiar la notacion de la ayuda con los signos < >
            if (linea.find('<') != string::npos || linea.find('>') != string::npos)
                cout << "  Pista: los signos < > de la ayuda no se escriben. "
                        "Ejemplo: usuario 10 (no usuario <10>)\n";
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

    // Persistencia: se guarda al salir (y tambien si se cierra la entrada)
    if (guardarDatos())
        cout << "Datos guardados en data/usuarios.txt y data/calificaciones.txt.\n";
    else
        cout << "Aviso: no se pudieron guardar los datos.\n";
    cout << "Hasta luego.\n";
}
