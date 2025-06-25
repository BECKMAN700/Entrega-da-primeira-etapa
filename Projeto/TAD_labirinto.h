#ifndef LABIRINTO_H
#define LABIRINTO_H
#include <stdbool.h>

typedef struct {
    char **mapa;
    int linhas;
    int colunas;
    int inicio, saida;
    int I_x, I_y, F_x, F_y;
} Labirinto;

bool carregar_labirinto(const char *arquivo, Labirinto *lab);
void limpar_labirinto(Labirinto *lab);






#endif