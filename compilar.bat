@echo off
REM Compila el motor y las pruebas. Funciona con doble clic o desde cualquier carpeta.
REM Uso:  compilar          -> compila motor.exe
REM       compilar pruebas  -> compila y ejecuta las pruebas de los modulos

cd /d "%~dp0"

set INC=-Isrc/contrato -Isrc/catalogo -Isrc/perfiles -Isrc/recomendador -Isrc/afinidad -Isrc/interprete -Isrc/persistencia
set FLAGS=-std=c++11 -Wall -Wextra %INC%

if /I "%1"=="pruebas" goto pruebas

g++ %FLAGS% src/catalogo/*.cpp src/perfiles/*.cpp src/recomendador/*.cpp src/afinidad/*.cpp src/interprete/*.cpp src/persistencia/*.cpp src/estadisticas/*.cpp -o motor.exe
if errorlevel 1 (
    echo ERROR: fallo la compilacion.
    exit /b 1
)
echo OK: motor.exe generado. Ejecuta: .\motor.exe
exit /b 0

:pruebas
g++ %FLAGS% src/recomendador/maxheap.cpp src/recomendador/recomendador.cpp tests/prueba_recomendador.cpp -o prueba_recomendador.exe || exit /b 1
g++ %FLAGS% src/afinidad/grafo.cpp src/afinidad/afinidad.cpp tests/prueba_afinidad.cpp -o prueba_afinidad.exe || exit /b 1
echo === prueba_recomendador ===
.\prueba_recomendador.exe || exit /b 1
echo === prueba_afinidad ===
.\prueba_afinidad.exe || exit /b 1