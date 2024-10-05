#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

#define SIZE 30  // Tamanho da floresta

// Definição da estrutura para representar cada célula da floresta
typedef struct {
    char estado;  // Estado da célula ('-', 'T', '@', '/')
    pthread_mutex_t lock;  // Mutex para sincronização
} Celula;


// Declaração da matriz de floresta
extern Celula floresta[SIZE][SIZE];

// Funções
void inicializar_floresta();
void imprimir_floresta();
void* sensor_thread(void* arg);
void* gerar_incendios(void* arg);
void* central_thread(void* arg);
void comunicar_incendio(int x, int y);
void gerenciar_incendios();

#endif
