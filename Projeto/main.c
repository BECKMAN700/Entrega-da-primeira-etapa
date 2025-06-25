#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "TAD_individuo.h"
#include "TAD_labirinto.h"
#include "TAD_populacao.h"
#include "TAD_config.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <mapa.txt> <config.yml>\n", argv[0]);
        return 1;
    }   

    srand(time(NULL));

    Labirinto lab;
    if (!carregar_labirinto(argv[1], &lab)) {
        printf("Erro ao carregar o labirinto!\n");
        return 1;
    }

    Config cfg;
    if (!carregar_config(argv[2], &cfg)) {
        printf("Erro ao carregar a configuração!\n");
        limpar_labirinto(&lab);
        return 1;
    }

    Populacao *pop = criar_meliantes(cfg.tamanho_populacao, cfg.max_tamanho_individuo, cfg.taxa_mutacao, cfg.taxa_elitismo, cfg.tipo_geracao, &lab);
    if (!pop) {
        printf("Erro ao criar a população!\n");
        limpar_labirinto(&lab);
        return 1;
    }

    FILE *log = fopen(cfg.saida_log, "w");
    if (!log) {
        printf("Erro ao criar o arquivo de log!\n");
        chacina(pop);
        limpar_labirinto(&lab);
        return 1;
    }

    for (int ger = 1; ger <= cfg.geracoes; ++ger) {
        avaliar_populacao(pop, &lab);

        Individuo *melhor = pop->meliante[0];
        fprintf(log, "%d,%.2lf,", ger, melhor->fitness);
        for (size_t i = 0; i < melhor->tamanho; i++) {
            fputc(melhor->DNA[i], log);
        }
        fputc('\n', log);

        evolucao(pop, &lab);
    }

    fclose(log);
    chacina(pop);
    limpar_labirinto(&lab);

    printf("Execução finalizada!\n");
    return 0;
}
