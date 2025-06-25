#include "TAD_labirinto.h"
#include <stdio.h>
#include <stdlib.h>


bool carregar_labirinto(const char *arquivo, Labirinto *lab) {
    
    // 1. Abrir o arquivo `caminho_arquivo`.
    FILE *arq = fopen(arquivo, "r");


    // verificação se o arquivo foi aberto 
    if (arq == NULL){
         printf("Erro no arquivo");
        return false;
    }
    else{
         printf("Arquivo aberto com sucesso");
    }

    // Ler as dimensões
    if(fscanf(arq,"%d %d", &lab->linhas, &lab->colunas) != 2){
        printf("Formato invalido: esperado 'linhas colunas'\n");
        fclose(arq);
        return false;
    }

    // Alocar memória para a struct Labirinto

    //Alocar linhas
    lab->mapa = malloc(lab->linhas *sizeof(char*));

    if(lab->mapa == NULL){
        perror("Erro ao alocar linhas");
        fclose(arq);
        return false;
    }

    // Alocar colunas
    for (int i = 0; i < lab->linhas; i++){
        lab->mapa[i] = malloc(lab->colunas *sizeof(char));
        if(lab->mapa[i] == NULL){
            perror("Erro ao alocar colunas");

            for (int j = 0; j< i; j++){
                free(lab->mapa[j]);
            }
            free(lab->mapa);
            fclose(arq);
            return false;
        }
    }

    // LER A GRADE DO LABIRINTO
    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            char c;
            if (fscanf(arq, "%c", &c) != 1) {   
                printf("Erro ao ler o mapa na posicao (%d, %d)\n", i, j);
                fclose(arq);
                for (int k = 0; k < lab->linhas; k++){
                    free(lab->mapa[k]);
                } 
                free(lab->mapa);
                return false;
            }
            if (c == '\n' || c == '\r') {
            j--;
            continue;
            }

            lab->mapa[i][j] = c;
        }

    }


    // Guardando as posições de 'I' e 'F'
     for (int i = 0; i < lab->linhas;i++){
        for (int j = 0; j < lab->colunas; j++){
            if(lab->mapa[i][j] == 'I'){
                lab->I_x = i;
                lab->I_y = j;
            }
            if(lab->mapa[i][j] == 'F'){
                lab->F_x = i;
                lab->F_y = j;
            }
        }
    }

    fclose(arq);    
    return true;
}

void limpar_labirinto(Labirinto *lab){
    if (lab == NULL || lab->mapa == NULL){
        return;
    } 

     // limpa a memoria das colunas
    for (int i = 0; i < lab->linhas; i++) {
        free(lab->mapa[i]);
    }

    //limpa a memoria das linhas
    free(lab->mapa);
    lab->mapa = NULL;
}