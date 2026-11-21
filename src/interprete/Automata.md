
Maquina de Estados (Automata Finito Determinista)

INICIO ──letra──> PALABRA ──letra/dígito──> PALABRA
   │                  └──otra cosa──> emite token, vuelve a INICIO
   ├──dígito──> NUMERO ──dígito──> NUMERO
   │                  └──otra cosa──> emite ENTERO, vuelve a INICIO
   ├──"──────> CADENA ──cualquier cosa──> CADENA
   │                  └──" ──> emite CADENA, vuelve a INICIO
   └──.──────> PUNTO ──.──> emite PUNTOS, vuelve a INICIO
