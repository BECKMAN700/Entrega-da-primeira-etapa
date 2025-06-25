#include <stdlib.h>
#include <stdio.h>
#include "TAD_populacao.h"
#include <string.h>

static size_t melhor_meliante(Populacao *povo){

    size_t melhor = 0;

    for(size_t i = 0; i < povo->tamanho; i++){
        if(povo->meliante[i]->fitness > povo->meliante[melhor]->fitness){
            melhor = i;
        }
    }

    return melhor;
}

// vai criar os meliantes para uma populaçao  
Populacao *criar_meliantes(size_t tamanho, size_t DNA_MAX, double mutacao_variacao, double populacao_preservada, char *tipo_geracao, const Labirinto *lab) {

    Populacao *povo = malloc(sizeof *povo);

    if (!povo) {
        printf("Erro na alocação para a população \n");
        return NULL;
    }

    povo->tamanho = tamanho;
    povo->DNA_MAX = DNA_MAX;
    povo->mutacao_variacao = mutacao_variacao;
    povo->populacao_preservada = populacao_preservada;
    povo->meliante = malloc(tamanho * sizeof *povo->meliante);

    if (!povo->meliante) { 
        printf("Erro na alocação do vetor de ponteiro");
        free(povo); 
        return NULL; 
    }
    for (size_t i = 0; i < tamanho; i++) {

        if (strcmp(tipo_geracao, "aleatorio") == 0) {
            povo->meliante[i] = criar_meliante(DNA_MAX);
        } else if (strcmp(tipo_geracao, "movimentos_validos") == 0) {
            povo->meliante[i] = criar_meliante_movimentos_validos(DNA_MAX, lab);
        } else {
            
            povo->meliante[i] = criar_meliante(DNA_MAX);
        }

        if (!povo->meliante[i]) {
            for (size_t j = 0; j < i; j++)
                destruir_meliante(povo->meliante[j]);
            free(povo->meliante);
            free(povo);
            return NULL;
        }
    }
    return povo;
}


// vai pegar cada meliante da população e avalair ele no labirinto
void avaliar_populacao(Populacao *povo, const Labirinto *lab){

    for (size_t i=0; i < povo->tamanho; i++){
        avaliar_meliante(povo->meliante[i], lab);
    }

    qsort(povo->meliante, povo->tamanho, sizeof(Individuo*), compara_fitness);
    
}

int compara_fitness(const void *a, const void *b) {

    Individuo *ia = *(Individuo **)a;
    Individuo *ib = *(Individuo **)b;

    if (ia->fitness < ib->fitness){
        return  1;
    } 
    if (ia->fitness > ib->fitness){
        return -1;
    } 
    return 0;
}


Individuo *selecionar_torneio(const Populacao *povo, size_t meliante_aleatorio){

    size_t maisForte = rand() % povo->tamanho;
    
    for (size_t i = 1; i< meliante_aleatorio; i++){

        size_t competidor01 = rand() % povo->tamanho;

        if(povo->meliante[competidor01]->fitness > povo->meliante[maisForte]->fitness){
            maisForte = competidor01;
        }
    }
    return povo->meliante[maisForte];
}

Individuo *cruzamento(const Individuo *pai, const Individuo *mae){

    
    if(pai->tamanho != mae->tamanho){
        printf("Pai e mãe não têm o mesmo tamanho de DNA. Crossover cancelado.\n");
        return NULL;
    }

    size_t n = pai->tamanho;
    size_t ponto_corte = 1 + rand() % (n - 1);

    Individuo *filho = criar_meliante(n);

    if(!filho){
        printf("Falha na cracao do filho");
        return NULL;
    }

    for(size_t i=0; i < n; i++){
    
        if(i < ponto_corte){    
            filho->DNA[i] = pai->DNA[i];
        }
        else{
            filho->DNA[i] = mae->DNA[i];
        }
    }
    return filho;
}

void evolucao(Populacao *povo, const Labirinto *lab){

    size_t quanti_povo = povo->tamanho;

    size_t elite = (size_t)(povo->populacao_preservada * quanti_povo);

    avaliar_populacao(povo, lab);
    
    Individuo **nova = malloc(quanti_povo * sizeof *nova);

    if(!nova){
        printf("Erro a alocar memoria para a nova geracao");
        return;
    }

    for (size_t i = 0; i < elite; i++) {

    nova[i] = criar_meliante(povo->DNA_MAX);    

    memcpy(nova[i]->DNA, povo->meliante[i]->DNA, povo->DNA_MAX * sizeof(char));

    nova[i]->tamanho = povo->meliante[i]->tamanho;
    nova[i]->fitness = povo->meliante[i]->fitness;

    }

    for (size_t i = elite; i < quanti_povo; i++) {
        Individuo *pai = selecionar_torneio(povo, 3);
        Individuo *mae = selecionar_torneio(povo, 3);
        Individuo *filho = cruzamento(pai, mae);
        mutar_meliante(filho, povo->mutacao_variacao);
        nova[i] = filho;
    }
    
    for (size_t i = 0; i < quanti_povo; i++) {
        destruir_meliante(povo->meliante[i]);
    }
    free(povo->meliante);

    povo->meliante = nova;

}

void chacina(Populacao *povo){

    if (!povo){
        return;
    } 
    for (size_t i = 0; i < povo->tamanho; i++) {
        destruir_meliante(povo->meliante[i]);
    }
    free(povo->meliante);
    free(povo);
}
