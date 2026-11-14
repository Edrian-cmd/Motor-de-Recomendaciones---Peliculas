#include "Gestor_Perfil.h"
#include "modulos.h"
#include <string>

namespace {

struct Historial {
    int id_pelicula;
    std::string accion;
    int estrellas;
    Historial* next;
    Historial* back;
};

struct Pila {
    int tipoAccion;
    int id_pelicula;
    int estrellasAnteriores;
    Pila* next;
};

struct Cola {
    int id_pelicula;
    Cola* next;
};

}

static Historial* cabezaHistorial = nullptr;
static int tamHistorial = 0;

static Pila* topePila = nullptr;
static int tamPila = 0;

static Cola* frenteCola = nullptr;
static Cola* finalCola = nullptr;
static int tamCola = 0;

static int idUsuarioActivo = 1;

static std::string diagnosticoDev() {
    if (modoDevActivo()) {
        return "\n[DEV] Perfiles -> Historial: " + std::to_string(tamHistorial) +
               " nodos | Pila: " + std::to_string(tamPila) +
               " nodos | Cola: " + std::to_string(tamCola) + " nodos";
    }
    return "";
}

std::string calificar(int idPelicula, int estrellas) {
    const Pelicula* pelicula = obtenerPelicula(idPelicula);
    if (pelicula == nullptr) {
        return "Error: La pelicula con ID " + std::to_string(idPelicula) + 
               " no existe en el catalogo." + diagnosticoDev();
    }

    if (estrellas < 1 || estrellas > 5) {
        return "Error: La calificacion debe estar en el rango de 1 a 5 estrellas." + diagnosticoDev();
    }

    Historial* actual = cabezaHistorial;
    Historial* nodoExistente = nullptr;
    while (actual != nullptr) {
        if (actual->id_pelicula == idPelicula) {
            nodoExistente = actual;
            break;
        }
        actual = actual->next;
    }

    int estrellasAnteriores = (nodoExistente != nullptr) ? nodoExistente->estrellas : 0;

    Pila* nuevoPila = new Pila();
    nuevoPila->tipoAccion = 1; 
    nuevoPila->id_pelicula = idPelicula;
    nuevoPila->estrellasAnteriores = estrellasAnteriores;
    nuevoPila->next = topePila;
    topePila = nuevoPila;
    tamPila++;

    if (nodoExistente != nullptr) {
        nodoExistente->estrellas = estrellas;
        nodoExistente->accion = "Recalificada con " + std::to_string(estrellas) + " estrellas";

        if (nodoExistente != cabezaHistorial) {
            if (nodoExistente->back != nullptr) {
                nodoExistente->back->next = nodoExistente->next;
            }
            if (nodoExistente->next != nullptr) {
                nodoExistente->next->back = nodoExistente->back;
            }
            nodoExistente->next = cabezaHistorial;
            nodoExistente->back = nullptr;
            if (cabezaHistorial != nullptr) {
                cabezaHistorial->back = nodoExistente;
            }
            cabezaHistorial = nodoExistente;
        }
    } else {
        Historial* nuevoHist = new Historial();
        nuevoHist->id_pelicula = idPelicula;
        nuevoHist->accion = "Calificada con " + std::to_string(estrellas) + " estrellas";
        nuevoHist->estrellas = estrellas;
        nuevoHist->next = cabezaHistorial;
        nuevoHist->back = nullptr;

        if (cabezaHistorial != nullptr) {
            cabezaHistorial->back = nuevoHist;
        }
        cabezaHistorial = nuevoHist;
        tamHistorial++;
    }

    registrarVoto(idPelicula, estrellas, estrellasAnteriores);
    grafoRegistrarCalificacion(usuarioActivo(), idPelicula, estrellas);

    return "Pelicula '" + pelicula->titulo + "' calificada exitosamente con " +
           std::to_string(estrellas) + " estrellas." + diagnosticoDev();
}

std::string deshacer() {
    if (topePila == nullptr) {
        return "No hay acciones para deshacer." + diagnosticoDev();
    }

    Pila* nodoDeshecho = topePila;
    topePila = topePila->next;
    tamPila--;

    int idPelicula = nodoDeshecho->id_pelicula;
    int estrellasAnteriores = nodoDeshecho->estrellasAnteriores;
    delete nodoDeshecho;

    Historial* actual = cabezaHistorial;
    Historial* nodoHist = nullptr;
    while (actual != nullptr) {
        if (actual->id_pelicula == idPelicula) {
            nodoHist = actual;
            break;
        }
        actual = actual->next;
    }

    int estrellasActuales = (nodoHist != nullptr) ? nodoHist->estrellas : 0;

    if (estrellasAnteriores == 0) {
        if (nodoHist != nullptr) {
            if (nodoHist->back != nullptr) {
                nodoHist->back->next = nodoHist->next;
            } else {
                cabezaHistorial = nodoHist->next;
            }

            if (nodoHist->next != nullptr) {
                nodoHist->next->back = nodoHist->back;
            }

            delete nodoHist;
            tamHistorial--;
        }
    } else {
        if (nodoHist != nullptr) {
            nodoHist->estrellas = estrellasAnteriores;
            nodoHist->accion = "Calificada con " + std::to_string(estrellasAnteriores) + " estrellas";
        }
    }

    registrarVoto(idPelicula, estrellasAnteriores, estrellasActuales);
    grafoRegistrarCalificacion(usuarioActivo(), idPelicula, estrellasAnteriores);

    const Pelicula* pelicula = obtenerPelicula(idPelicula);
    std::string nombrePelicula = (pelicula != nullptr) ? pelicula->titulo : ("ID " + std::to_string(idPelicula));

    std::string respuesta;
    if (estrellasAnteriores == 0) {
        respuesta = "Accion deshecha: Se elimino la calificacion de '" + nombrePelicula + "'.";
    } else {
        respuesta = "Accion deshecha: Se restauro la calificacion de '" + nombrePelicula + 
                    "' a " + std::to_string(estrellasAnteriores) + " estrellas.";
    }

    return respuesta + diagnosticoDev();
}

std::string verHistorial() {
    if (cabezaHistorial == nullptr) {
        return "El historial esta vacio." + diagnosticoDev();
    }

    std::string salida = "--- Historial de Calificaciones (Usuario " + 
                         std::to_string(usuarioActivo()) + ") ---\n";

    Historial* actual = cabezaHistorial;
    int contador = 1;
    while (actual != nullptr) {
        const Pelicula* pelicula = obtenerPelicula(actual->id_pelicula);
        std::string titulo = (pelicula != nullptr) ? pelicula->titulo : "Pelicula ID " + std::to_string(actual->id_pelicula);

        salida += std::to_string(contador) + ". [ID " + std::to_string(actual->id_pelicula) + 
                  "] " + titulo + " - " + std::to_string(actual->estrellas) + " estrellas (" + 
                  actual->accion + ")\n";

        actual = actual->next;
        contador++;
    }

    return salida + diagnosticoDev();
}

std::string descubrir() {
    while (frenteCola != nullptr) {
        Cola* temp = frenteCola;
        frenteCola = frenteCola->next;
        delete temp;
    }
    finalCola = nullptr;
    tamCola = 0;

    int idsDestino[10];
    int cantidad = topN(10, idsDestino);

    for (int i = 0; i < cantidad; ++i) {
        Cola* nuevo = new Cola();
        nuevo->id_pelicula = idsDestino[i];
        nuevo->next = nullptr;

        if (frenteCola == nullptr) {
            frenteCola = nuevo;
            finalCola = nuevo;
        } else {
            finalCola->next = nuevo;
            finalCola = nuevo;
        }
        tamCola++;
    }

    return "Cola de descubrimientos cargada con " + std::to_string(cantidad) + 
           " recomendaciones." + diagnosticoDev();
}

std::string siguiente() {
    if (frenteCola == nullptr) {
        return "No hay peliculas en la cola de descubrimientos. Usa 'descubrir' para cargar nuevas." + diagnosticoDev();
    }

    Cola* extraido = frenteCola;
    int idPelicula = extraido->id_pelicula;
    frenteCola = frenteCola->next;
    if (frenteCola == nullptr) {
        finalCola = nullptr;
    }
    delete extraido;
    tamCola--;

    const Pelicula* pelicula = obtenerPelicula(idPelicula);
    std::string salida;
    if (pelicula != nullptr) {
        salida = "Siguiente descubrimiento: [ID " + std::to_string(pelicula->id) + "] " +
                 pelicula->titulo + " (" + std::to_string(pelicula->anio) + ") - Genero: " + pelicula->genero;
    } else {
        salida = "Siguiente descubrimiento: Pelicula ID " + std::to_string(idPelicula);
    }

    return salida + diagnosticoDev();
}


int usuarioActivo() {
    return idUsuarioActivo;
}

int estrellasDelActivo(int idPelicula) {
    Historial* actual = cabezaHistorial;
    while (actual != nullptr) {
        if (actual->id_pelicula == idPelicula) {
            return actual->estrellas;
        }
        actual = actual->next;
    }
    return 0; 
}

int calificacionesDelActivo(Calificacion* destino, int max) {
    if (destino == nullptr || max <= 0) {
        return 0;
    }

    int copiadas = 0;
    Historial* actual = cabezaHistorial;
    while (actual != nullptr && copiadas < max) {
        destino[copiadas].idUsuario = usuarioActivo();
        destino[copiadas].idPelicula = actual->id_pelicula;
        destino[copiadas].estrellas = actual->estrellas;
        copiadas++;
        actual = actual->next;
    }

    return copiadas;
}


void perfilCargarCalificacion(int idPelicula, int estrellas) {
    if (estrellas < 1 || estrellas > 5) {
        return;
    }

    Historial* actual = cabezaHistorial;
    while (actual != nullptr) {
        if (actual->id_pelicula == idPelicula) {
            actual->estrellas = estrellas;
            actual->accion = "Calificada con " + std::to_string(estrellas) + " estrellas";
            return;
        }
        actual = actual->next;
    }

    Historial* nuevoHist = new Historial();
    nuevoHist->id_pelicula = idPelicula;
    nuevoHist->accion = "Calificada con " + std::to_string(estrellas) + " estrellas";
    nuevoHist->estrellas = estrellas;
    nuevoHist->next = cabezaHistorial;
    nuevoHist->back = nullptr;

    if (cabezaHistorial != nullptr) {
        cabezaHistorial->back = nuevoHist;
    }
    cabezaHistorial = nuevoHist;
    tamHistorial++;
}

void perfilCambiarUsuario(int idUsuario) {
    limpiarMemoriaPerfiles();
    idUsuarioActivo = idUsuario;
}


void limpiarMemoriaPerfiles() {
    while (cabezaHistorial != nullptr) {
        Historial* temp = cabezaHistorial;
        cabezaHistorial = cabezaHistorial->next;
        delete temp;
    }
    tamHistorial = 0;

    while (topePila != nullptr) {
        Pila* temp = topePila;
        topePila = topePila->next;
        delete temp;
    }
    tamPila = 0;

    while (frenteCola != nullptr) {
        Cola* temp = frenteCola;
        frenteCola = frenteCola->next;
        delete temp;
    }
    finalCola = nullptr;
    tamCola = 0;
}


namespace {

struct GuardiaMemoriaPerfiles {
    ~GuardiaMemoriaPerfiles() { limpiarMemoriaPerfiles(); }
};

GuardiaMemoriaPerfiles guardiaMemoriaPerfiles;

}
