#include "Gestor_Perfil.h"

void agregarAlHistorial(Historial*& head, int id, string accion) {
    Historial* nuevoNodo = new Historial();
    nuevoNodo->id_pelicula = id;
    nuevoNodo->accion = accion;
    
    nuevoNodo->next = head;     
    nuevoNodo->back = nullptr;  

    if (head != nullptr) {
        head->back = nuevoNodo;
    }
    
    head = nuevoNodo;
    
    cout << "[Historial] Pelicula ID " << id << " agregada en memoria real.\n";
}

void mostrarHistorial(Historial* head) {
    if (head == nullptr) {
        cout << "[Historial] El historial esta vacio.\n";
        return;
    }
    
    Historial* temp = head;
    cout << "\n--- Historial del Usuario ---\n";
    
    while (temp != nullptr) {
        cout << "-> ID: " << temp->id_pelicula << " | Accion: " << temp->accion << "\n";
        temp = temp->next; 
    }
    cout << "\n";
}

void registrarAccion(Pila*& tope, string accion) {
    Pila* nuevoNodo = new Pila();
    nuevoNodo->descripcion_accion = accion;
    nuevoNodo->next = tope;
    
    tope = nuevoNodo;
    cout << "[Pila] Accion apilada en memoria real: " << accion << "\n";
}

void deshacerUltimaAccion(Pila*& tope) {
    if (tope == nullptr) {
        cout << "[Pila] No hay acciones para deshacer.\n";
        return;
    }

    Pila* nodoABorrar = tope;
    tope = tope->next;

    cout << "[Pila] Deshaciendo accion en memoria real: " << nodoABorrar->descripcion_accion << "\n";
    delete nodoABorrar;
}

void encolarPelicula(Cola*& frente, Cola*& final, int id) {
    Cola* nuevoNodo = new Cola();
    nuevoNodo->id_pelicula = id;
    nuevoNodo->next = nullptr;

    if (frente == nullptr) {
        frente = nuevoNodo;
        final = nuevoNodo;
    } else {
        final->next = nuevoNodo;
        final = nuevoNodo;
    }
    
    cout << "[Cola] Pelicula ID " << id << " encolada en memoria real.\n";
}

void mostrarSiguienteDescubrimiento(Cola*& frente) {
    if (frente == nullptr) {
        cout << "[Cola] No hay peliculas en la cola de descubrimiento.\n";
        return;
    }

    Cola* nodoABorrar = frente;
    cout << "[Cola] -> Descubrimiento: Te recomendamos la pelicula ID " << nodoABorrar->id_pelicula << "\n";
    
    frente = frente->next;
    delete nodoABorrar;
}

void cargarMocksGestorPerfiles(Historial*& cabeza, Pila*& tope, Cola*& frente, Cola*& final) {
    cout << "\n[SISTEMA] Inyectando datos de prueba (Mocks) en el Gestor de Perfiles...\n";
    
    agregarAlHistorial(cabeza, 104, "Calificada con 5 estrellas");
    registrarAccion(tope, "Calificó ID 104");
    
    agregarAlHistorial(cabeza, 205, "Vista");
    registrarAccion(tope, "Vio ID 205");

    encolarPelicula(frente, final, 301);
    encolarPelicula(frente, final, 450);
    
    cout << "[SISTEMA] Mocks cargados exitosamente.\n\n";
}