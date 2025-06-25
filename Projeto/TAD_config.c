#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TAD_config.h"

// Função simples para ler config
bool carregar_config(const char *caminho, Config *cfg) {
    FILE *arq = fopen(caminho, "r");
    if (!arq) {
        printf("Erro ao abrir o arquivo de configuração!\n");
        return false;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arq)) {
        if (linha[0] == '#' || linha[0] == '\n') continue; // ignora comentários/linhas vazias

        if (strstr(linha, "taxa_elitismo:"))
            sscanf(linha, "taxa_elitismo: %lf", &cfg->taxa_elitismo);
        else if (strstr(linha, "taxa_mutacao:"))
            sscanf(linha, "taxa_mutacao: %lf", &cfg->taxa_mutacao);
        else if (strstr(linha, "tamanho_populacao:"))
            sscanf(linha, "tamanho_populacao: %d", &cfg->tamanho_populacao);
        else if (strstr(linha, "geracoes:"))
            sscanf(linha, "geracoes: %d", &cfg->geracoes);
        else if (strstr(linha, "tipo_geracao:"))
            sscanf(linha, "tipo_geracao: %s", cfg->tipo_geracao);
        else if (strstr(linha, "max_tamanho_individuo:"))
            sscanf(linha, "max_tamanho_individuo: %d", &cfg->max_tamanho_individuo);
        else if (strstr(linha, "saida_log:")) {
            char *ptr = strchr(linha, ':');
            if (ptr) {
                ptr++;
                while (*ptr == ' ' || *ptr == '\t') ptr++;
                sscanf(ptr, "%s", cfg->saida_log);
            }
        }
    }
    fclose(arq);
    return true;
}
