@echo off
REM Compila el motor y las pruebas. Ejecutar desde la raiz del repositorio.
REM Uso:  compilar          -> compila motor.exe
REM       compilar pruebas  -> compila y ejecuta las pruebas de los modulos

cd /d "%~dp0"

if /I "%1"=="pruebas" goto pruebas

g++ -std=c++11 -Wall -Wextra -Isrc src\*.cpp -o motor.exe
if errorlevel 1 (
    echo ERROR: fallo la compilacion.
    exit /b 1
)
echo OK: motor.exe generado. Ejecuta: .\motor.exe
exit /b 0

:pruebas
g++ -std=c++11 -Wall -Wextra -Isrc src\maxheap.cpp src\recomendador.cpp tests\prueba_recomendador.cpp -o prueba_recomendador.exe || exit /b 1
g++ -std=c++11 -Wall -Wextra -Isrc src\grafo.cpp src\afinidad.cpp tests\prueba_afinidad.cpp -o prueba_afinidad.exe || exit /b 1
echo === prueba_recomendador ===
.\prueba_recomendador.exe || exit /b 1
echo === prueba_afinidad ===
.\prueba_afinidad.exe || exit /b 1
