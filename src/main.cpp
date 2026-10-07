/*
 * Proyecto: Motor Inteligente de Recomendaciones de Películas
 * Curso: Estructuras de Datos
 * Grupo: 5
 * Archivo principal (main.cpp)
 */

#include <iostream>
#include <string>

using namespace std;

int main() {
    bool sistemaActivo = true;
    string comando;

    cout << "=========================================================" << endl;
    cout << "   MOTOR DE RECOMENDACIONES DE PELICULAS - GRUPO 5" << endl;
    cout << "=========================================================" << endl;
    cout << "Sistema iniciado. Ingrese 'ayuda' para ver opciones o 'salir' para apagar." << endl;

    // Bucle principal del Intérprete de Comandos (CMD)
    while (sistemaActivo) {
        cout << "\nusuario@motorCMD> ";
        getline(cin, comando);

        // Procesamiento básico de comandos (Stub)
        if (comando == "salir") {
            cout << "Guardando datos y cerrando el sistema..." << endl;
            sistemaActivo = false;
        } 
        else if (comando == "ayuda") {
            cout << "Lista de comandos disponibles (Modulos en construccion):" << endl;
            cout << " - buscar   : Consulta el catalogo (Hash/AVL)" << endl;
            cout << " - calificar: Agrega interaccion al historial (Listas/Pilas)" << endl;
            cout << " - recomendar: Genera Top-N (Max-Heap)" << endl;
            cout << " - modo_dev : Activa metricas de rendimiento" << endl;
        } 
        else if (comando != "") {
            cout << "[DEV] El comando '" << comando << "' aun no esta conectado a ninguna funcion." << endl;
        }
    }

    return 0;
}
