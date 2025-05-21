#include "labirinto_ga.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 1. Carregar o labirinto
    Labirinto* labirinto = labirinto_carregar("labirinto.txt");
    if (!labirinto) {
        printf("Erro: Nao foi possivel carregar o labirinto!\n");
        printf("Verifique se o arquivo labirinto.txt existe no diretorio.\n");
        return 1;
    }

    printf("Labirinto carregado com sucesso!\n");
    printf("Tamanho: %d linhas x %d colunas\n", labirinto->linhas, labirinto->colunas);
    printf("Inicio: (%d, %d)\n", labirinto->inicio.x, labirinto->inicio.y);
    printf("Fim: (%d, %d)\n\n", labirinto->fim.x, labirinto->fim.y);

    // 2. Configuração do algoritmo genético
    int tamanho_populacao = 50;       // Número de indivíduos
    int max_geracoes = 200;           // Máximo de gerações
    float taxa_mutacao = 0.02;        // 2% de chance de mutação

    printf("Configuracao do Algoritmo Genetico:\n");
    printf("- Tamanho da populacao: %d\n", tamanho_populacao);
    printf("- Maximo de geracoes: %d\n", max_geracoes);
    printf("- Taxa de mutacao: %.2f\n\n", taxa_mutacao);

    // 3. Executar o algoritmo genético
    printf("Executando o algoritmo genetico...\n");
    algoritmo_genetico(labirinto, tamanho_populacao, max_geracoes, taxa_mutacao);

    // 4. Liberar memória do labirinto
    for (int i = 0; i < labirinto->linhas; i++) {
        free(labirinto->mapa[i]);
    }
    free(labirinto->mapa);
    free(labirinto);

    return 0;
}