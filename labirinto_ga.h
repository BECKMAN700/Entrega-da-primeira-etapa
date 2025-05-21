#ifndef LABIRINTO_GA_H
#define LABIRINTO_GA_H

#include "TLinkedList.h"
#include <stdbool.h>

// Direções possíveis para os movimentos
typedef enum {
    CIMA,
    BAIXO,
    ESQUERDA,
    DIREITA
} Direcao;

// Estrutura para representar uma posição no labirinto
typedef struct {
    int x;
    int y;
} Posicao;

// Estrutura para representar um indivíduo (solução candidata)
typedef struct {
    TLinkedList* movimentos;    // Sequência de movimentos
    int fitness;                // Qualidade da solução
} Individuo;

// Estrutura para representar o labirinto
typedef struct {
    char** mapa;                // Matriz representando o labirinto
    int linhas;                 // Número de linhas
    int colunas;                // Número de colunas
    Posicao inicio;             // Posição inicial (S)
    Posicao fim;                // Posição final (E)
} Labirinto;

// Funções principais
Labirinto* labirinto_carregar(const char* arquivo);
Individuo* individuo_criar(int tamanho_max_movimentos);
void individuo_destruir(Individuo* individuo);
Individuo* individuo_copiar(Individuo* original); // <--- NOVA FUNÇÃO
int calcular_fitness(Individuo* individuo, Labirinto* labirinto);
void simular_caminho(Individuo* individuo, Labirinto* labirinto, Posicao* final);
Individuo* cruzamento(Individuo* pai1, Individuo* pai2);
void mutacao(Individuo* individuo, float taxa_mutacao);
void algoritmo_genetico(Labirinto* labirinto, int tamanho_populacao, int max_geracoes, float taxa_mutacao);

// Funções auxiliares
int distancia_manhattan(Posicao a, Posicao b);
bool posicao_valida(Labirinto* labirinto, Posicao pos);
bool eh_parede(Labirinto* labirinto, Posicao pos);
Direcao direcao_aleatoria();
void imprimir_labirinto(Labirinto* labirinto);

#endif