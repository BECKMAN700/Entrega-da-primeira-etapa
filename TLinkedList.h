#ifndef TLINKED_LIST_H
#define TLINKED_LIST_H
#include<stdbool.h>
 
typedef struct _no {
    int info;
    struct _no *prox;
} TNo;

typedef struct _list {
    TNo* inicio;
} TLinkedList;

 typedef struct _list TLinkedList;
 
 /**
  * Cria instâncias da lista simplesmente encadeada.
  * @return TLinkedList* ponteiro para a lista alocada
  * */
 TLinkedList* list_create();
 
 /**
  * Tenta fazer a inserção no início da lista. 
  * Por exemplo, se a sequência de inserção for 1, 2, 3, 
  * a lista será [3] -> [2] -> [1]
  * @param TLinkedList* ponteiro para uma instância da lista
  * @param int informação a ser armazenada na lista
  * @return bool informando o sucesso da operação
  */
 bool list_insert_begin(TLinkedList*, int);
 
 /**
  * Tenta fazer a inserção no fim da lista. 
  * Por exemplo, se a sequência de inserção for 1, 2, 3, 
  * a lista será [1] -> [2] -> [3]
  * @param TLinkedList* ponteiro para uma instância da lista
  * @param int informação a ser armazenada na lista
  * @return bool informando o sucesso da operação
  */
 bool list_insert_end(TLinkedList*, int);
 /**
  * Faz a impressão da lista para checagem.
  * @param TLinkedList* ponteiro para uma instância da lista
  */
 
  /**
  * Tenta deletar o elemento do início. 
  * Por exemplo, se a lista é [1] -> [2] -> [3], apos a chamada, 
  * a Lista se tornará [2] -> [3]
  * @param TLinkedList* ponteiro para uma instância da lista
  * @return bool informando o sucesso da operação
  */
 bool list_delete_begin(TLinkedList*);
 void list_print(TLinkedList*);
 /**
  * Concatenta duas listas.
  * @param TLinkedList* lista 1
  * @param TLinkedList* lista 2
  * @return TLinkedList* instância da terceira lista concatenada
  */
 TLinkedList* list_concatenate(TLinkedList*, TLinkedList*);
 
 
 /**
  * Tenta remover os n primeiros elementos da lista.
  * @param TLinkedList* lista 1
  * @param unsigned int n 
  * @return bool informando o sucesso da operação
  */
 bool list_delete_n(TLinkedList*, unsigned int);
 #endif