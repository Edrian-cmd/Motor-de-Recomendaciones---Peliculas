Regla 1: Nadie programa en la rama main. Cada uno debe abrir VS Code, ir a la esquina inferior izquierda (donde dice main), hacer clic y seleccionar "Create new branch" (Crear nueva rama). Deben ponerle un nombre que identifique su tarea, como david-grafos o piero-hash.
Regla 2: Trabajeremos usando las herramientas visuales de Visual Studio Code o GitHub Desktop para hacer sus "Commits". No intentemos escribir comandos extraños en la consola negra hasta que dominen el flujo visual.
Todo lo relacionado a Github y VS Code ya seria cuestion de aprender a usarlo utiliazando y preguntadole a la IA. Esta es la forma mas rapida y facil de poder avanzar el proyecto

# Motor de Recomendaciones de Peliculas - Grupo 5

## Compilar y ejecutar

Abre en VS Code la carpeta del repositorio (`Motor-de-Recomendaciones---Peliculas`) y en la terminal:

    .\compilar.bat              (genera motor.exe)
    .\motor.exe

    .\compilar.bat pruebas      (compila y ejecuta las pruebas de los modulos)

Para repetir una sesion de ejemplo: `Get-Content tests\sesion_demo.txt | .\motor.exe`

Al escribir `salir` se guardan `data/usuarios.txt` y `data/calificaciones.txt`.

## Comandos

Lo que va entre `< >` se reemplaza por un valor, **sin escribir los signos** (`usuario 4`, no `usuario <4>`).

    buscar id <n> | buscar genero "<texto>" | buscar anio <a>[..<b>]
    calificar id <n> <1-5> | deshacer | historial
    recomendar top <n> [genero "<texto>"] | explicar id <n>
    descubrir | siguiente
    afinidad usuario <n> | usuario <n>
    estadisticas | comparar id <n> | comparar anio <a>[..<b>]
    dev on | dev off | ayuda | salir

## Modulos (una carpeta por modulo dentro de src/)

| Carpeta | Integrante | Estructuras |
|---|---|---|
| contrato/ | 5 | modulos.h (funciones entre modulos), modelo.h (Pelicula, Calificacion) |
| catalogo/ | 1 | Tabla Hash (sondeo lineal), Arbol AVL por anio |
| perfiles/ | 2 | Lista doble (historial), Pila (deshacer), Cola (descubrir) |
| recomendador/ | 3 | Max-Heap para el Top-N, promedio bayesiano |
| afinidad/ | 4 | Grafo con listas de adyacencia, Jaccard |
| interprete/ | 5 | AFD (lexer) + parser descendente recursivo |
| persistencia/ | 5 | Listas enlazadas de usuarios y calificaciones, carga y guardado |
| estadisticas/ | 5 | Resumen del catalogo y del usuario (usa el Max-Heap) |

Mas detalle en `docs/CONTEXTO_PROYECTO.md` y reglas de trabajo en `CLAUDE.md`.

## Datos

- data/peliculas.txt: `ID,Titulo,Genero,Anio` (702 peliculas, sin duplicados).
- data/usuarios.txt: `ID,Nombre` (50 usuarios; el usuario 1 es el activo al iniciar).
- data/calificaciones.txt: `IDUsuario,IDPelicula,Estrellas` (~1450 calificaciones con gustos por genero).
