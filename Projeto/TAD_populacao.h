#ifndef POPULACAO_H
#define POLULACAO_H

#include <stddef.h>
#include "TAD_individuo.h"
#include "TAD_labirinto.h"

typedef struct{

    Individuo **meliante;
    size_t tamanho; //quantos indivíduos estão nessa população
    size_t DNA_MAX; //tamanho do DNA
    double mutacao_variacao; //taxa de mutação
    double populacao_preservada; //porcentagem da população que será preservada

}Populacao;

// vai criar a populaçao com o quantidade de meliantes, movimentação e as taxa de mutação e os melhores individuos
Populacao *criar_meliantes(size_t tamanho, size_t DNA_MAX, double mutacao_variacao, double populacao_preservada, char *tipo_geracao, const Labirinto *lab);

//coloca e avalia os meliante no labirinto
void avaliar_populacao(Populacao *povo, const Labirinto *lab);


int compara_fitness(const void *a, const void *b);

//escolhe meliantes aleatoriamente
Individuo *selecionar_torneio(const Populacao *povo, size_t meliante_aleatorio);

//cruzamento do pai e da mae para ter um filho com o DNA dos dois
Individuo *cruzamento(const Individuo *pai, const Individuo *mae);

//vai evoluir a populacao
void evolucao(Populacao *povo, const Labirinto *lab);

// vai assasinar toda a população
void chacina(Populacao *povo);

#endif