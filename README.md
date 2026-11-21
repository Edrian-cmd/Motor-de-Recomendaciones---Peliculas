Regla 1: Nadie programa en la rama main. Cada uno debe abrir VS Code, ir a la esquina inferior izquierda (donde dice main), hacer clic y seleccionar "Create new branch" (Crear nueva rama). Deben ponerle un nombre que identifique su tarea, como david-grafos o piero-hash.
Regla 2: Trabajeremos usando las herramientas visuales de Visual Studio Code o GitHub Desktop para hacer sus "Commits". No intentemos escribir comandos extraños en la consola negra hasta que dominen el flujo visual.
Todo lo relacionado a Github y VS Code ya seria cuestion de aprender a usarlo utiliazando y preguntadole a la IA. Esta es la forma mas rapida y facil de poder avanzar el proyecto

# Motor de Recomendaciones de Peliculas - Grupo 5

## Compilar y ejecutar (desde la raiz del repositorio)

    compilar              (genera motor.exe)
    motor.exe

    compilar pruebas      (compila y ejecuta las pruebas de los modulos)

O a mano:

    g++ -std=c++11 -Wall -Wextra -Isrc src/*.cpp -o motor.exe

## Comandos

    buscar id <n> | buscar genero "<texto>" | buscar anio <a>[..<b>]
    calificar id <n> <1-5> | deshacer | historial
    recomendar top <n> [genero "<texto>"] | explicar id <n>
    descubrir | siguiente
    afinidad usuario <n> | usuario <n>
    dev on | dev off | ayuda | salir

## Modulos (contrato en src/modulos.h, datos en src/modelo.h)

| Integrante | Archivos | Estructuras |
|---|---|---|
| 1 Bibliotecario | bibliotecario.cpp, hash.*, avl.* | Tabla Hash (sondeo lineal), Arbol AVL por anio |
| 2 Gestor de Perfiles | Gestor_Perfil.* | Lista doble (historial), Pila (deshacer), Cola (descubrir) |
| 3 Matematico | recomendador.*, maxheap.* | Max-Heap para el Top-N, promedio bayesiano |
| 4 Analista Social | afinidad.*, grafo.* | Grafo con listas de adyacencia, Jaccard |
| 5 Interprete | lexer.*, parser.*, ast.*, token.*, interprete.* | AFD + parser descendente recursivo |
| Integracion | calificaciones.cpp | Carga data/calificaciones.txt y cambio de usuario |

## Datos

- data/peliculas.txt: `ID,Titulo,Genero,Anio` (702 peliculas, sin duplicados).
- data/calificaciones.txt: `idUsuario,idPelicula,estrellas` (40 usuarios con gustos
  distintos; el usuario 1 es el activo al iniciar). Las lineas que empiezan con `#` se ignoran.
