# CLAUDE.md — Motor de Recomendaciones de Películas (EDD, UNMSM)

Este archivo lo lee automáticamente Claude Code al abrir el repositorio.
Cada integrante trabaja con su propia cuenta de Claude; **todas las IAs deben seguir estas reglas** para que los merges no se rompan.

El contexto completo (estado, contrato entre módulos, tareas por integrante) está en:

@docs/CONTEXTO_PROYECTO.md

## Quién es quién

| # | Integrante | Rol | Carpeta | Rama de trabajo |
|---|-----------|-----|---------|-----------------|
| 1 | Aaron  | Bibliotecario: catálogo, Tabla Hash, AVL | `src/catalogo/` | `Aaron-Bibliotecario` |
| 2 | Imanol | Gestor de Perfiles: lista doble, pila, cola | `src/perfiles/` | `modulo-gestor-perfiles` |
| 3 | Piero  | Matemático: Max-Heap, puntaje, Top-N | `src/recomendador/` | `Piero-(Motor-de-Recomendación-Personalizado)` |
| 4 | Oliver | Analista Social: grafo + Jaccard | `src/afinidad/` | `modulo-grafos` |
| 5 | Edrian | Líder técnico: lexer/parser CMD, integración, persistencia | `src/interprete/`, `src/contrato/`, `src/persistencia/`, `src/estadisticas/` | `EdrianLC` |

Al iniciar una sesión, pregunta al usuario **qué integrante es** si no lo dice, y trabaja solo en sus archivos.

## Estructura de carpetas

```
src/
  contrato/      modulos.h, modelo.h            (contrato v2, de Edrian)
  catalogo/      hash.*, avl.*, bibliotecario.cpp
  perfiles/      Gestor_Perfil.*
  recomendador/  maxheap.*, recomendador.*
  afinidad/      grafo.*, afinidad.*
  interprete/    main.cpp, interprete.*, lexer.*, parser.*, token.*, ast.*, gramatica.md
  persistencia/  persistencia.*                  (usuarios y calificaciones en data/)
  estadisticas/  estadisticas.cpp
tests/           pruebas aisladas de cada modulo (cada una con su main)
data/            peliculas.txt, usuarios.txt, calificaciones.txt
docs/            CONTEXTO_PROYECTO.md
```

Los `#include` usan solo el nombre del archivo (`#include "modulos.h"`); cada carpeta se pasa al compilador con `-I`. No escribir rutas relativas en los includes.

## Reglas obligatorias para la IA

1. **Nunca trabajar en `main`.** Antes de editar, comprobar la rama con `git branch --show-current`.
2. **Solo editar los archivos del integrante** (tabla "Dueños de archivos" en `docs/CONTEXTO_PROYECTO.md`). Si hace falta cambiar un archivo de otro, NO editarlo: escribir la propuesta en el chat para que el usuario la coordine con el dueño.
3. **El contrato es sagrado.** `src/contrato/modulos.h` y `src/contrato/modelo.h` son de Edrian. No cambiar firmas existentes. Si se necesita una función nueva del contrato, proponerla; se agrega con un PR pequeño y separado a `main`.
4. **No reformatear** código ajeno ni el propio de forma masiva (indentación, renombrados, mover funciones). Diffs pequeños = merges sin conflictos.
5. **Sin librerías de estructuras de la STL en los módulos de EDD**: las estructuras (hash, AVL, listas, pilas, colas, heap, grafo) se implementan con punteros y `new/delete`. `std::string`, `<sstream>`, `<iostream>`, `<fstream>`, `<chrono>` sí se usan. (El lexer usa `unordered_map`/`vector` como tabla de símbolos; ya está aceptado.) Prohibido ordenar con Bubble Sort o QuickSort para el ranking.
6. **Las funciones del contrato devuelven `std::string`**, no imprimen con `cout`. Solo `interprete.cpp` imprime.
7. **Modo desarrollador:** la información de diagnóstico (colisiones, rotaciones, intercambios, recorridos, tiempos) se agrega al string devuelto con el prefijo `[DEV]`, y solo si `modoDevActivo()` es verdadero.
8. **C++11**, identificadores y comentarios en español **sin tildes ni ñ** (evita problemas de codificación entre Windows/Linux). Cada `new` debe tener su `delete` (destructores).
9. **Diferenciar hechos de propuestas**: lo marcado como PROPUESTA en el documento de contexto todavía no está decidido por el grupo; no asumirlo como definitivo.
10. **Antes de terminar** cualquier tarea: compilar el proyecto completo (comando abajo) y correr los tests del módulo. No dejar el repo sin compilar.
11. El código debe poder ser **explicado por cualquiera de los 5** ante el profesor: preferir soluciones claras a soluciones ingeniosas, y comentar el porqué y la complejidad (O(...)) de cada operación de la estructura.

## Compilar y probar

Desde la raíz del repositorio (g++ con MinGW/MSYS2 en Windows):

```bash
compilar.bat              # genera motor.exe
compilar.bat pruebas      # compila y ejecuta los tests de los modulos
./motor.exe
```

A mano (lo mismo que hace `compilar.bat`):

```bash
g++ -std=c++11 -Wall -Wextra -Isrc/contrato -Isrc/catalogo -Isrc/perfiles -Isrc/recomendador -Isrc/afinidad -Isrc/interprete -Isrc/persistencia src/catalogo/*.cpp src/perfiles/*.cpp src/recomendador/*.cpp src/afinidad/*.cpp src/interprete/*.cpp src/persistencia/*.cpp src/estadisticas/*.cpp -o motor.exe
```

Tests aislados (cada uno tiene su propio `main`, NO se compilan con `src/interprete/main.cpp`):

```bash
g++ -std=c++11 -Wall -Wextra -Isrc/contrato -Isrc/recomendador src/recomendador/maxheap.cpp src/recomendador/recomendador.cpp tests/prueba_recomendador.cpp -o prueba_recomendador.exe
g++ -std=c++11 -Wall -Wextra -Isrc/contrato -Isrc/afinidad src/afinidad/grafo.cpp src/afinidad/afinidad.cpp tests/prueba_afinidad.cpp -o prueba_afinidad.exe
```

Los `.exe` y `.o` están en `.gitignore`: nunca subirlos.
Ojo: `salir` guarda en `data/usuarios.txt` y `data/calificaciones.txt`. Si solo estabas probando, no subas esos cambios (`git restore data/`).

## Flujo de Git para cada sesión

```bash
git switch <mi-rama>
git fetch origin
git merge origin/main          # traer lo último ANTES de programar
# ... trabajar, compilar, probar ...
git add <solo mis archivos>
git commit -m "Int.N: descripcion corta"
git push origin <mi-rama>
```

Luego abrir un Pull Request de `<mi-rama>` hacia `main` en GitHub. Edrian (o el equipo) lo revisa y lo integra.
Si `git merge origin/main` da conflicto en un archivo que no es tuyo, quedarse con la versión de `main` para ese archivo.
