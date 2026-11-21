# Contexto del proyecto, estado actual y plan de trabajo (v3)

Documento vivo. Última revisión: 21-nov-2026, sobre la rama `integracion-final`
(main + ramas de Aaron, Piero y Oliver del 17/20-nov + integración de Edrian).
Reemplaza a la v2 (17-nov, PDF).

Leyenda: **HECHO** = verificado en el código. **FALTA** = pendiente. **PROPUESTA** = sugerencia que el grupo puede ajustar.
Las reglas para la IA y para Git están en `CLAUDE.md`.

## 1. Qué es el proyecto

Motor Inteligente de Recomendaciones de Películas en C++ con punteros y memoria dinámica, para Estructuras de Datos (UNMSM).
Interfaz: intérprete de comandos con lexer (AFD) y parser descendente propios (aporte de Lenguajes y Compiladores).
`dev on` muestra el estado interno de cada estructura y tiempos. Calificaciones de 1 a 5 estrellas.
Entregable 3: funciona sin datos de prueba, con persistencia real.

## 2. Situación actual

La rama `integracion-final` **compila sin errores ni warnings** (`-Wall -Wextra`) y los tests
`prueba_recomendador` y `prueba_afinidad` pasan. `grep -ri mock src/` sale vacío.

## 3. Estado por integrante

| Integrante | Estado | Resumen |
|---|---|---|
| Aaron (Int.1) | **HECHO** | Hash con `Pelicula` de modelo.h, `obtener`, arreglo dinámico de carga, `registrarVoto` O(1), contadores [DEV], destructores. Integración: búsqueda de género sin mayúsculas, rango de años invertido, `compararPorId/compararPorAnio` (+ `contieneId`/`contarPorAnio` en el AVL). |
| Imanol (Int.2) | **HECHO** | Lista doble, pila de deshacer, cola de descubrir. Integración: `siguiente` salta las películas ya calificadas. |
| Piero (Int.3) | **HECHO** | Max-Heap, 4 componentes, promedio bayesiano, `topN`. Integración: se quitó `recomendadorModoDev` (solo `modoDevActivo`) y se agregó `recomendadorReiniciar()` para `cambiarUsuario`. |
| Oliver (Int.4) | **HECHO** | Grafo real, Jaccard sobre gustadas (>= 4) por mezcla O(\|A\|+\|B\|), caché de k vecinos, `puntajeColaborativo`. Integración: se quitó `afinidadModoDev`. |
| Edrian (Int.5) | **HECHO** | Contrato v2, `modoDevActivo`, persistencia, comando `usuario`, `estadisticas`, `comparar`, carpetas por módulo, `compilar.bat`. |

## 4. Contrato v2 (`src/contrato/`)

`modelo.h`: `Pelicula {id, titulo, genero, anio, ratingPromedio, numVotos}` y `Calificacion {idUsuario, idPelicula, estrellas}`.
El orden de los campos importa (los tests inicializan con llaves).

`modulos.h` (quién implementa cada función):

```cpp
// Int.1 Aaron (catalogo/)
bool cargarCatalogo(const std::string& ruta);
std::string buscarPorId(int id);  std::string buscarPorGenero(const std::string&);  std::string buscarPorAnio(int, int);
const Pelicula* obtenerPelicula(int id);  int totalPeliculas();  const Pelicula* peliculaPorIndice(int i);
void registrarVoto(int idPelicula, int estrellasNuevas, int estrellasAnteriores);
std::string compararPorId(int id);  std::string compararPorAnio(int desde, int hasta);   // nuevo (PR 6)
// Int.2 Imanol (perfiles/)
std::string calificar(int, int); deshacer(); verHistorial(); descubrir(); siguiente();
int usuarioActivo();  int estrellasDelActivo(int);  int calificacionesDelActivo(Calificacion*, int);
// Int.3 Piero (recomendador/)
std::string recomendar(int n, const std::string& genero);  std::string explicar(int id);  int topN(int n, int* ids);
// Int.4 Oliver (afinidad/)
std::string afinidad(int usuario);  void grafoRegistrarCalificacion(int u, int p, int e);  double puntajeColaborativo(int p);
// Int.5 Edrian
bool modoDevActivo();  bool cargarDatos();  bool guardarDatos();  std::string cambiarUsuario(int);  std::string estadisticas();
```

`perfilCargarCalificacion` y `perfilCambiarUsuario` siguen en `Gestor_Perfil.h` (no en el contrato), como pedía la v2.

Convención del "0 estrellas" (obligatoria):

| Situación | registrarVoto(id, nuevas, anteriores) | grafoRegistrarCalificacion(u, id, e) |
|---|---|---|
| Primera calificación | (id, 5, 0) | (u, id, 5) |
| Recalificar de 3 a 5 | (id, 5, 3) | (u, id, 5) |
| Deshacer una calificación nueva | (id, 0, 5) | (u, id, 0) → quitar del grafo |

## 5. Persistencia (`src/persistencia/`)

- `data/usuarios.txt` (`ID,Nombre`, 50 usuarios) y `data/calificaciones.txt` (`IDUsuario,IDPelicula,Estrellas`, ~1450 filas).
  Generados con gustos por género y semilla fija; usan los géneros exactos de `peliculas.txt`.
- En memoria: lista enlazada de usuarios y lista enlazada de las `Calificacion` de los usuarios **no activos**.
- `cargarDatos()`: catálogo → usuarios → por cada calificación `registrarVoto(p,e,0)` y `grafoRegistrarCalificacion(u,p,e)`;
  si es del activo `perfilCargarCalificacion`, si no va a la lista. Filas repetidas se toman como recalificación.
- `guardarDatos()` (al `salir`): reescribe ambos archivos con la lista + `calificacionesDelActivo()`.
- `cambiarUsuario(id)`: el activo pasa a la lista, `perfilCambiarUsuario(id)`, se sacan de la lista las del nuevo.
  Si el id no existe se crea "Usuario N" (arranque en frío).
- `data/peliculas.txt`: se quitaron 48 películas duplicadas (mismo título y año). Quedan 702.

## 6. Comandos

`buscar id|genero|anio`, `calificar`, `deshacer`, `historial`, `recomendar top N [genero]`, `explicar id`,
`descubrir`/`siguiente`, `afinidad usuario N`, `usuario N`, `estadisticas`, `comparar id N`, `comparar anio A..B`,
`dev on|off`, `ayuda`, `salir`. Gramática en `src/interprete/gramatica.md`.

## 7. Lo que FALTA (no es código del motor)

- ☐ Piero: ajustar pesos y `VOTOS_CONFIANZA` mirando resultados reales y justificarlos.
- ☐ (Opcional, Piero) Top-N con min-heap de tamaño N, O(m log N), para sumarlo a `comparar`.
- ☐ (Opcional, Aaron) índice género → lista de `Pelicula*` para que `buscar genero` no sea O(n), o justificarlo en la defensa.
- ☐ (Opcional, Imanol) `tests/prueba_perfiles.cpp`.
- ☐ Todos: en `docs/`, una ficha por estructura (problema, por qué esa estructura, complejidad, alternativa descartada).
- ☐ Todos: defensa cruzada, guion y grabación del video del Entregable 3.

## 8. Dueños de archivos (solo el dueño los edita)

| Archivos | Dueño |
|---|---|
| `src/interprete/*`, `src/contrato/*`, `src/persistencia/*`, `src/estadisticas/*`, `data/usuarios.txt`, `data/calificaciones.txt`, `compilar.bat` | Edrian |
| `src/catalogo/*`, `data/peliculas.txt` | Aaron |
| `src/perfiles/*`, `tests/prueba_perfiles.cpp` | Imanol |
| `src/recomendador/*`, `tests/prueba_recomendador.cpp` | Piero |
| `src/afinidad/*`, `tests/prueba_afinidad.cpp` | Oliver |
| `README.md`, `CLAUDE.md`, `docs/` | Todos, solo vía PR |

## 9. Cómo trabajar con distintas cuentas de Claude sin romper los merges

1. Un integrante, una rama, sus archivos.
2. Antes de programar: `git fetch origin` y `git merge origin/main` en tu rama.
3. Si necesitas algo de otro módulo que no está en `modulos.h`, pídelo en el grupo.
4. PRs pequeños, siempre compilando con `compilar.bat`.
5. No reformatear, renombrar ni mover código existente sin acordarlo.
6. Conflicto en un archivo que no es tuyo: quédate con la versión de `main`.
