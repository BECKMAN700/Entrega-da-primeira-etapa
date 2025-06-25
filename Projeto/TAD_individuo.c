#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>  
#include <time.h>  
#include <string.h>  
#include "TAD_config.h"
#include "TAD_labirinto.h"
#include "TAD_individuo.h"


Individuo *criar_meliante(size_t Max_DNA) {
    Individuo *meliante = malloc(sizeof *meliante);
    if (!meliante) {
        printf("Falha na locação do meliante");
        return NULL;
    }
    meliante->tamanho = Max_DNA;
    meliante->fitness = 0.0;
    meliante->DNA = malloc(Max_DNA * sizeof *meliante->DNA);
    if (!meliante->DNA) {
        printf("Erro a alocar memoria para o DNA");
        free(meliante);
        return NULL;
    }
    static const char direcoes[4] = {'N', 'S', 'L', 'O'};
    for (size_t i = 0; i < Max_DNA; i++) {
        meliante->DNA[i] = direcoes[rand() % 4];
    }
    return meliante;
}

Individuo *criar_meliante_movimentos_validos(size_t Max_DNA, const Labirinto *lab) {
    Individuo *meliante = malloc(sizeof *meliante);
    if (!meliante) return NULL;
    meliante->tamanho = Max_DNA;
    meliante->fitness = 0.0;
    meliante->DNA = malloc(Max_DNA * sizeof *meliante->DNA);
    if (!meliante->DNA) {
        free(meliante);
        return NULL;
    }
    int x = lab->I_x;
    int y = lab->I_y;
    for (size_t i = 0; i < Max_DNA; i++) {
        char opcoes[4];
        int n = 0;
        if (x > 0 && lab->mapa[x-1][y] != '#') opcoes[n++] = 'N';
        if (x < lab->linhas-1 && lab->mapa[x+1][y] != '#') opcoes[n++] = 'S';
        if (y > 0 && lab->mapa[x][y-1] != '#') opcoes[n++] = 'O';
        if (y < lab->colunas-1 && lab->mapa[x][y+1] != '#') opcoes[n++] = 'L';
        if (n == 0) {
            static const char todas[4] = {'N','S','L','O'};
            meliante->DNA[i] = todas[rand() % 4];
        } else {
            meliante->DNA[i] = opcoes[rand() % n];
            switch(meliante->DNA[i]) {
                case 'N': x--; break;
                case 'S': x++; break;
                case 'L': y++; break;
                case 'O': y--; break;
            }
        }
    }
    return meliante;
}



void avaliar_meliante(Individuo *meliante, const Labirinto *lab){

    //Posiçoes do meliante, inicio e fim;
    int x = lab->I_x;
    int y = lab->I_y;

    //variavel que vai receber o fitniss
    double desempenho_meliante = 0.0;

    //guarda o local que ja passou
    int visitado[lab->linhas][lab->colunas];

    //faz com que todos os individuos comecem do zero(zera a memoria de visitante)
    memset(visitado, 0, sizeof(visitado));

    //Vai percorrer todo o DNA e lendo os movimentos do meliante
    for(size_t i=0; i< meliante->tamanho; i++){

        //variavel que pega os movimentos
        char movimento_atual = meliante->DNA[i];

        //direção que o meliante vai
        int direcao_X = 0, direcao_Y=0;

        //ver qual e o movimento do meliante
        if(movimento_atual == 'N'){
            direcao_X--;
        }
        else{ if(movimento_atual == 'S'){
                direcao_X++;
            }
            else{ if(movimento_atual == 'L'){
                    direcao_Y++;
                }
                else{
                    direcao_Y--;
                }
            }
        }

        //onde o meliante foi parar depois do movimento
        int novas_coordenadas_X = x + direcao_X;
        int novas_coordenadas_Y = y + direcao_Y;

        //verifica se esse movimento e valido 
        if(novas_coordenadas_X < 0 || novas_coordenadas_X >= lab->linhas 
           || novas_coordenadas_Y < 0 || novas_coordenadas_Y >= lab->colunas 
           || lab->mapa[novas_coordenadas_X][novas_coordenadas_Y] == '#'){

            //se não for valido o meliante toma penalidade
            desempenho_meliante -= 2.0;
        }
        else{// se for valido o movimento e contabilizado

            int distancia_anterior = abs(lab->F_x - x) + abs(lab->F_y - y);
            int distancia_nova = abs(lab->F_x - novas_coordenadas_X) + abs(lab->F_y - novas_coordenadas_Y);

            if (distancia_nova < distancia_anterior) desempenho_meliante += 2.0; // Aproximou do F
            else desempenho_meliante += 0.5; // Andou, mas não se aproximou

            if (visitado[novas_coordenadas_X][novas_coordenadas_Y]) desempenho_meliante -= 0.5; // Penaliza repetição

            x = novas_coordenadas_X;
            y = novas_coordenadas_Y;
            
        }

        //verifica se o meliante ja chegou no fim do labirinto
        if(x == lab->F_x && y == lab->F_y){
            desempenho_meliante += 100.0;
            break;
        } 
    }

    //se não ja chegou no fim, verifica o quao longe ta do fim, usando o metodo Manhattan
    int distanciaX_fim = abs(lab->F_x - x);
    int distanciaY_fim = abs(lab->F_y - y);

    //vai recebdo penalidade se vai pra mais longe ou recebe agrado se vai pra mais perto do fim
    desempenho_meliante -= (distanciaX_fim + distanciaY_fim)*0.05;

    //recebe o desempenho do meliante
    meliante->fitness = desempenho_meliante;
}

void mutar_meliante(Individuo *meliante, double variacao){

    //Percorre todo o os movimentos(DNA) do individuo
    for (size_t i = 0; i < meliante->tamanho; i++){

        //Aqui que vai dar a probabilidade, tem a chance de entrar no if, se entrar o meliante e mutado
        if((rand() / (double)RAND_MAX) < variacao){

            //variavel para pegar a opçoes
            static const char movimentos[4] = {'N', 'S', 'L', 'O'};
            
            //Aqui um individuo vai receber um movimento aleatorio
            meliante->DNA[i] = movimentos[rand() % 4];
        }
    }
}

void destruir_meliante(Individuo *meliante){

    if(!meliante){
        printf("\n ==========Erro ao destruir meliante========== \n");
        return;
    }
    free(meliante->DNA);
    free(meliante);
}
