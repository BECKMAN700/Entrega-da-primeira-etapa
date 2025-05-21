#include "labirinto_ga.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

// Inicializa o gerador de números aleatórios
void inicializar_aleatorio() {
    srand(time(NULL));
}

// Carrega o labirinto a partir de um arquivo
Labirinto* labirinto_carregar(const char* arquivo) {
    FILE* file = fopen(arquivo, "r");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return NULL;
    }

    Labirinto* labirinto = malloc(sizeof(Labirinto));
    if (!labirinto) {
        fclose(file);
        return NULL;
    }

    // Contar linhas e colunas
    labirinto->linhas = 0;
    labirinto->colunas = 0;
    char linhaLida[1024];
    
    while (fgets(linhaLida, sizeof(linhaLida), file)) {
        labirinto->linhas++;
        int tamanhoLinha = strlen(linhaLida);
        if (linhaLida[tamanhoLinha-1] == '\n') tamanhoLinha--;
        if (tamanhoLinha > labirinto->colunas) {
            labirinto->colunas = tamanhoLinha;
        }
    }

    // Alocar memória para o mapa
    labirinto->mapa = malloc(labirinto->linhas * sizeof(char*));
    if (!labirinto->mapa) { // Adicionar verificação de alocação aqui
        fclose(file);
        free(labirinto);
        return NULL;
    }
    for (int i = 0; i < labirinto->linhas; i++) {
        labirinto->mapa[i] = malloc(labirinto->colunas * sizeof(char));
        if (!labirinto->mapa[i]) { // Adicionar verificação de alocação e liberar memória já alocada
            for (int j = 0; j < i; j++) {
                free(labirinto->mapa[j]);
            }
            free(labirinto->mapa);
            free(labirinto);
            fclose(file);
            return NULL;
        }
    }

    // Voltar ao início do arquivo e ler o conteúdo
    rewind(file);
    for (int i = 0; i < labirinto->linhas; i++) {
        if (!fgets(linhaLida, sizeof(linhaLida), file)) break;
        
        for (int j = 0; j < labirinto->colunas; j++) {
            if (j < strlen(linhaLida) && linhaLida[j] != '\n') {
                labirinto->mapa[i][j] = linhaLida[j];
                
                // Verificar se é a posição inicial ou final
                if (linhaLida[j] == 'S') {
                    labirinto->inicio.x = i;
                    labirinto->inicio.y = j;
                } else if (linhaLida[j] == 'E') {
                    labirinto->fim.x = i;
                    labirinto->fim.y = j;
                }
            } else {
                labirinto->mapa[i][j] = ' ';
            }
        }
    }

    fclose(file);
    return labirinto;
}

// Cria um novo indivíduo com movimentos aleatórios
Individuo* individuo_criar(int tamanho_max_movimentos) {
    Individuo* individuo = malloc(sizeof(Individuo));
    if (!individuo) return NULL;
    
    int tamanho = tamanho_max_movimentos;
    
    individuo->movimentos = list_create();
    if (!individuo->movimentos) {
        free(individuo);
        return NULL;
    }
    
    for (int i = 0; i < tamanho; i++) {
        Direcao dir = direcao_aleatoria();
        list_insert_end(individuo->movimentos, dir);
    }   
    
    individuo->fitness = 0;
    return individuo;
}

// Libera a memória de um indivíduo
void individuo_destruir(Individuo* individuo) {
    if (individuo) {
        if (individuo->movimentos) {
            // Precisamos limpar a lista de movimentos
            while (list_delete_begin(individuo->movimentos));
            free(individuo->movimentos);
            individuo->movimentos = NULL; // Previne dangling pointer
        }
        free(individuo);
    }
}

// NOVA FUNÇÃO: Copia um indivíduo (cópia profunda)
Individuo* individuo_copiar(Individuo* original) {
    if (!original) return NULL;

    Individuo* copia = malloc(sizeof(Individuo));
    if (!copia) return NULL;

    copia->fitness = original->fitness;
    copia->movimentos = list_create(); // Cria uma nova lista para a cópia

    if (!copia->movimentos) {
        free(copia);
        return NULL;
    }

    TNo* no_original = original->movimentos->inicio;
    while (no_original) {
        list_insert_end(copia->movimentos, no_original->info); // Copia os movimentos
        no_original = no_original->prox;
    }

    return copia;
}

// Calcula a distância de Manhattan entre duas posições
int distancia_manhattan(Posicao a, Posicao b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Verifica se uma posição é válida no labirinto
bool posicao_valida(Labirinto* labirinto, Posicao pos) {
    return pos.x >= 0 && pos.x < labirinto->linhas && 
           pos.y >= 0 && pos.y < labirinto->colunas;
}

// Verifica se uma posição é uma parede
bool eh_parede(Labirinto* labirinto, Posicao pos) {
    // Certifica-se de que a posição é válida antes de tentar acessar mapa
    if (!posicao_valida(labirinto, pos)) {
        return true; // Considerar fora dos limites como parede para evitar segfault
    }
    return labirinto->mapa[pos.x][pos.y] == '#';
}

// Gera uma direção aleatória
Direcao direcao_aleatoria() {
    return rand() % 4;  // 0-3 correspondem às direções do enum
}

// Simula o caminho de um indivíduo no labirinto
void simular_caminho(Individuo* individuo, Labirinto* labirinto, Posicao* final) {
    Posicao atual = labirinto->inicio;
    TNo* no = individuo->movimentos->inicio;
    
    while (no != NULL) {
        Direcao dir = no->info;
        Posicao proxima = atual;
        
        // Calcula próxima posição baseada na direção
        switch (dir) {
            case CIMA:      proxima.x--; break;
            case BAIXO:     proxima.x++; break;
            case ESQUERDA: proxima.y--; break;
            case DIREITA:   proxima.y++; break;
        }
        
        // Se a próxima posição for válida e não for parede, move
        if (posicao_valida(labirinto, proxima)) {
            if (!eh_parede(labirinto, proxima)) {
                atual = proxima;
            }
        }
        
        // Se chegou ao fim, para
        if (atual.x == labirinto->fim.x && atual.y == labirinto->fim.y) {
            break;
        }
        
        no = no->prox;
    }
    
    *final = atual;
}

// Calcula o fitness de um indivíduo
int calcular_fitness(Individuo* individuo, Labirinto* labirinto) {
    Posicao final;
    simular_caminho(individuo, labirinto, &final);
    
    // Distância até o objetivo
    int dist = distancia_manhattan(final, labirinto->fim);
    
    // Fitness é inversamente proporcional à distância
    individuo->fitness = -dist;
    return individuo->fitness;
}

// Realiza o cruzamento entre dois indivíduos
Individuo* cruzamento(Individuo* pai1, Individuo* pai2) {
    Individuo* filho = malloc(sizeof(Individuo));
    if (!filho) return NULL;
    
    filho->movimentos = list_create();
    if (!filho->movimentos) {
        free(filho);
        return NULL;
    }
    
    // Ponto de corte aleatório
    int tamanho_pai1 = 0, tamanho_pai2 = 0;
    TNo* no = pai1->movimentos->inicio;
    while (no) { tamanho_pai1++; no = no->prox; }
    no = pai2->movimentos->inicio;
    while (no) { tamanho_pai2++; no = no->prox; }
    
    // Garante que o corte seja válido mesmo se uma lista for vazia
    int min_tamanho = (tamanho_pai1 < tamanho_pai2 ? tamanho_pai1 : tamanho_pai2);
    if (min_tamanho == 0) { // Se um dos pais é vazio, o filho também será ou terá tamanho 0
        // Pode-se optar por retornar NULL ou um indivíduo vazio, ou com movimentos aleatórios
        // Por simplicidade, vou retornar um indivíduo vazio aqui.
        filho->fitness = 0;
        return filho;
    }
    int corte = rand() % min_tamanho;
    
    // Copia primeira parte do pai1
    no = pai1->movimentos->inicio;
    for (int i = 0; i < corte && no; i++) {
        list_insert_end(filho->movimentos, no->info);
        no = no->prox;
    }
    
    // Copia segunda parte do pai2
    no = pai2->movimentos->inicio;
    for (int i = 0; i < corte && no; i++) no = no->prox;
    while (no) {
        list_insert_end(filho->movimentos, no->info);
        no = no->prox;
    }
    
    filho->fitness = 0;
    return filho;
}

// Aplica mutação em um indivíduo
void mutacao(Individuo* individuo, float taxa_mutacao) {
    TNo* no = individuo->movimentos->inicio;
    while (no) {
        if ((float)rand() / RAND_MAX < taxa_mutacao) {
            no->info = direcao_aleatoria();
        }
        no = no->prox;
    }
}

// Seleção por torneio
Individuo* selecionar_por_torneio(Individuo** populacao, int tamanho_populacao, int tamanho_torneio) {
    Individuo* melhor = NULL;
    int melhor_fitness = -__INT_MAX__; // Usar o menor valor possível para fitness

    for (int i = 0; i < tamanho_torneio; i++) {
        int idx = rand() % tamanho_populacao;
        // Certifica-se de que populacao[idx] não é NULL antes de acessar
        if (populacao[idx] != NULL) {
            if (!melhor || populacao[idx]->fitness > melhor_fitness) {
                melhor = populacao[idx];
                melhor_fitness = populacao[idx]->fitness;
            }
        }
    }
    
    return melhor;
}

// Algoritmo genético principal
void algoritmo_genetico(Labirinto* labirinto, int tamanho_populacao, int max_geracoes, float taxa_mutacao) {
    inicializar_aleatorio();
    
    // Calcular tamanho máximo de movimentos (distância de Manhattan ou fixo)
    int distancia_m = distancia_manhattan(labirinto->inicio, labirinto->fim);
    int tamanho_max_movimentos = distancia_m * 2;   // Ou valor fixo como 50
    if (tamanho_max_movimentos == 0) tamanho_max_movimentos = 50; // Evita tamanho 0 se inicio == fim

    // Criar população inicial
    Individuo** populacao = malloc(tamanho_populacao * sizeof(Individuo*));
    if (!populacao) {
        fprintf(stderr, "Erro: Falha ao alocar memoria para a populacao inicial.\n");
        return;
    }
    for (int i = 0; i < tamanho_populacao; i++) {
        populacao[i] = individuo_criar(tamanho_max_movimentos);
        if (!populacao[i]) {
            fprintf(stderr, "Erro: Falha ao criar individuo %d. Liberando memoria e saindo.\n", i);
            for (int j = 0; j < i; j++) {
                individuo_destruir(populacao[j]);
            }
            free(populacao);
            return;
        }
        calcular_fitness(populacao[i], labirinto);
    }
    
    // Executar as gerações
    for (int geracao = 0; geracao < max_geracoes; geracao++) {
        // Criar nova população
        Individuo** nova_populacao = malloc(tamanho_populacao * sizeof(Individuo*));
        if (!nova_populacao) {
            fprintf(stderr, "Erro: Falha ao alocar memoria para a nova populacao.\n");
            // Destruir a população atual antes de sair
            for (int i = 0; i < tamanho_populacao; i++) {
                individuo_destruir(populacao[i]);
            }
            free(populacao);
            return;
        }
        
        // Elitismo: manter o melhor indivíduo
        int melhor_idx = 0;
        for (int i = 1; i < tamanho_populacao; i++) {
            // Certifica-se de que os indivíduos não são NULL antes de comparar
            if (populacao[i] && populacao[melhor_idx]) {
                if (populacao[i]->fitness > populacao[melhor_idx]->fitness) {
                    melhor_idx = i;
                }
            } else if (populacao[i] && !populacao[melhor_idx]) { // Se o atual é válido e o melhor_idx é NULL
                 melhor_idx = i;
            }
        }
        
        // CÓPIA PROFUNDA do indivíduo elitista
        nova_populacao[0] = individuo_copiar(populacao[melhor_idx]);
        if (!nova_populacao[0]) {
            fprintf(stderr, "Erro: Falha ao copiar individuo elitista. Saindo.\n");
            // Limpar tudo e sair
            for (int i = 0; i < tamanho_populacao; i++) individuo_destruir(populacao[i]);
            free(populacao);
            free(nova_populacao);
            return;
        }
        
        // Preencher o resto da nova população
        for (int i = 1; i < tamanho_populacao; i++) {
            // Seleção
            Individuo* pai1 = selecionar_por_torneio(populacao, tamanho_populacao, 5);
            Individuo* pai2 = selecionar_por_torneio(populacao, tamanho_populacao, 5);
            
            // Tratamento para pais NULL (se a seleção falhar por algum motivo)
            if (!pai1 || !pai2) {
                // Tentar criar um indivíduo aleatório ou pular este slot
                nova_populacao[i] = individuo_criar(tamanho_max_movimentos);
                if (!nova_populacao[i]) {
                    fprintf(stderr, "Erro: Falha ao criar individuo de fallback. Saindo.\n");
                    // Limpar tudo e sair
                    for (int k = 0; k <= i; k++) individuo_destruir(nova_populacao[k]); // Destruir os que já foram alocados na nova_populacao
                    free(nova_populacao);
                    for (int k = 0; k < tamanho_populacao; k++) individuo_destruir(populacao[k]);
                    free(populacao);
                    return;
                }
            } else {
                // Cruzamento
                Individuo* filho = cruzamento(pai1, pai2);
                if (!filho) {
                    fprintf(stderr, "Erro: Falha no cruzamento. Criando individuo aleatorio.\n");
                    filho = individuo_criar(tamanho_max_movimentos); // Fallback para criar aleatório
                    if (!filho) {
                         fprintf(stderr, "Erro: Falha ao criar individuo de fallback. Saindo.\n");
                         // Limpar tudo e sair
                        for (int k = 0; k <= i; k++) individuo_destruir(nova_populacao[k]);
                        free(nova_populacao);
                        for (int k = 0; k < tamanho_populacao; k++) individuo_destruir(populacao[k]);
                        free(populacao);
                        return;
                    }
                }
                
                // Mutação
                mutacao(filho, taxa_mutacao);
                
                // Calcular fitness
                calcular_fitness(filho, labirinto);
                
                nova_populacao[i] = filho;
            }
        }
        
        // Substituir a população antiga
        // Agora, todos os indivíduos da população antiga (populacao) podem ser destruídos,
        // pois o elitista foi copiado profundamente.
        for (int i = 0; i < tamanho_populacao; i++) {
            individuo_destruir(populacao[i]);
        }
        free(populacao);
        populacao = nova_populacao;
        
        // Verificar se encontramos a solução
        Posicao final;
        // Garantir que populacao[0] não é NULL antes de simular
        if (populacao[0]) {
            simular_caminho(populacao[0], labirinto, &final);
            if (final.x == labirinto->fim.x && final.y == labirinto->fim.y) {
                printf("Solucao encontrada na geracao %d!\n", geracao);
                break;
            }
        } else {
            fprintf(stderr, "Aviso: populacao[0] e NULL. Possivel problema na criacao de individuos.\n");
        }
        
        // Log do progresso
        if (geracao % 10 == 0) {
            if (populacao[0]) {
                printf("Geracao %d - Melhor fitness: %d\n", geracao, populacao[0]->fitness);
            } else {
                printf("Geracao %d - Melhor fitness: N/A (populacao[0] e NULL)\n", geracao);
            }
        }
    }
    
    // Imprimir a melhor solução encontrada
    printf("\nMelhor solucao encontrada:\n");
    Posicao final;
    if (populacao[0]) {
        simular_caminho(populacao[0], labirinto, &final);
        printf("Posicao final: (%d, %d)\n", final.x, final.y);
        printf("Fitness: %d\n", populacao[0]->fitness);
    } else {
        printf("Nenhuma solucao valida encontrada ou populacao[0] e NULL.\n");
    }
    
    // Liberar memória final da população
    for (int i = 0; i < tamanho_populacao; i++) {
        individuo_destruir(populacao[i]);
    }
    free(populacao);
}

// Função para imprimir o labirinto
void imprimir_labirinto(Labirinto* labirinto) {
    if (!labirinto || !labirinto->mapa) {
        printf("Labirinto invalido para impressao.\n");
        return;
    }
    for (int i = 0; i < labirinto->linhas; i++) {
        for (int j = 0; j < labirinto->colunas; j++) {
            printf("%c", labirinto->mapa[i][j]);
        }
        printf("\n");
    }
}