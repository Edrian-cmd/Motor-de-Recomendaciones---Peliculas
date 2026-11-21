#ifndef RECOMENDADOR_H
#define RECOMENDADOR_H

// Integrante 3: Matematico (Motor de Recomendacion Personalizado)
// recomendar() y explicar() estan declaradas en modulos.h (contrato con el
// interprete). Aqui va lo propio de este modulo.

// Ranking para otros modulos (contrato v2). Copia en idsDestino los ids de las
// n mejores peliculas no calificadas por el usuario activo, de mayor a menor
// puntaje, y devuelve cuantas copio. Lo usa descubrir() del Int.2.
// La misma firma esta en modulos.h; declararla dos veces igual es valido en C++.
int topN(int n, int* idsDestino);

// Interruptor local del diagnostico del Max-Heap. Se mantiene porque el
// interprete actual lo llama en "dev on"/"dev off"; el modulo tambien respeta
// modoDevActivo() global del contrato v2. Se podra borrar cuando Edrian deje
// solo el interruptor global.
void recomendadorModoDev(bool activo);

#endif
