#ifndef INDIVIDUO_H
#define INDIVIDUO_H
#include <stddef.h>
#include "TAD_labirinto.h"
#include "TAD_config.h"
// A estrutura do individuo
typedef struct{

    char *DNA; // sequencia de movimentos do individuo
    size_t tamanho;// quantos movimentos do individuo
    double fitness;// o quanto o indivuo parou perto do fim

}Individuo;

// cria o individuo com o maximo de movimentos aleatorio,
//que serio o cromossomo do individuo que forma o DNA
Individuo *criar_meliante(size_t Max_DNA); 

Individuo *criar_meliante_movimentos_validos(size_t Max_DNA, const Labirinto *lab);

// vai calcular o fitness(quanto o individuo chegou perto do fim),
//simulando seu DNA(sequencia de movimentos) no labirinto
void avaliar_meliante(Individuo *meliante, const Labirinto *lab);

//Tem a chance de pegar o DNA(sequencia de movimentos) do meliante e modificar aleatoriamente
//A sequencia de movimentos do meliante
void mutar_meliante(Individuo *meliante, double variacao);

//Vai liberar a memoria alocada para esse meliante
void destruir_meliante(Individuo *meliante);


#endif