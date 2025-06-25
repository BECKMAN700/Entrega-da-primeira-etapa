#ifndef CONFIG_H
#define CONFIG_H
#include <stdbool.h>

typedef struct {
    double taxa_elitismo;
    double taxa_mutacao;
    int tamanho_populacao;
    int geracoes;
    char tipo_geracao[64];
    int max_tamanho_individuo;
    char saida_log[256];
} Config;

bool carregar_config(const char *caminho, Config *cfg);

#endif
