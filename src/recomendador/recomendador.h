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

// Olvida la ultima lista de recomendar() (la usa explicar()). Se llama al
// cambiar de usuario activo, porque esa lista era del usuario anterior.
void recomendadorReiniciar();

#endif
