#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  // Para sleep
#include "functions.h"

// Inicializa a matriz da floresta
Celula floresta[SIZE][SIZE];

// Função para inicializar a floresta
void inicializar_floresta() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            floresta[i][j].estado = 'T';  // Todas as células começam com 'T' (monitoradas por sensores)
            pthread_mutex_init(&floresta[i][j].lock, NULL);  // Inicializa os mutexes para cada célula
        }
    }
}

// Função para imprimir o estado da floresta
void imprimir_floresta() {
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", floresta[i][j].estado);  // Imprime o estado de cada célula
        }
        printf("\n");
    }
    printf("---------------------------------------------------------\n");
    printf("\n");
}

// Função que simula a comunicação entre nós vizinhos ao detectar um incêndio
void comunicar_incendio(int x, int y) {
    // Verifica se a célula está na borda
    if (x == 0 || x == SIZE-1 || y == 0 || y == SIZE-1) {
        printf("Incêndio detectado na borda [%d, %d]. Notificando central.\n", x, y);
    } else {
        printf("Incêndio detectado em [%d, %d]. Notificando vizinhos.\n", x, y);
    }
}

// Função que será executada pelas threads dos sensores
void* sensor_thread(void* arg) {
    int* pos = (int*)arg;
    int x = pos[0];
    int y = pos[1];
    free(pos);

    while (1) {
        pthread_mutex_lock(&floresta[x][y].lock);
        if (floresta[x][y].estado == '@') {
            comunicar_incendio(x, y);
        }
        pthread_mutex_unlock(&floresta[x][y].lock);
        sleep(1);  // Pausa de 1 segundo entre as leituras
    }
    return NULL;
}

// Função da thread que gera incêndios aleatórios
void* gerar_incendios(void* arg) {
    while (1) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        pthread_mutex_lock(&floresta[x][y].lock);
        if (floresta[x][y].estado == 'T') {
            floresta[x][y].estado = '@';  // Coloca fogo na célula
            printf("Incêndio iniciado em [%d, %d]\n", x, y);
            imprimir_floresta();  // Imprime a matriz da floresta atualizada
        }
        pthread_mutex_unlock(&floresta[x][y].lock);

        sleep(3);  // Pausa de 3 segundos entre incêndios
    }
    return NULL;
}

// Função que gerencia o combate aos incêndios (central)
void* central_thread(void* arg) {
    while (1) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                pthread_mutex_lock(&floresta[i][j].lock);
                if (floresta[i][j].estado == '@') {
                    printf("Combate ao fogo iniciado em [%d, %d].\n", i, j);
                    floresta[i][j].estado = '/';  // Marca como célula queimada
                    imprimir_floresta();  // Atualiza a floresta
                }
                pthread_mutex_unlock(&floresta[i][j].lock);
            }
        }
        sleep(1);  // Pausa de 1 segundo entre cada iteração de gerenciamento
    }
    return NULL;
}
