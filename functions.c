#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  
#include "functions.h"

// Inicializa a matriz da floresta
Celula floresta[SIZE][SIZE];

// Direção do vento (dx, dy)
int wind_dx = 1;  // Exemplo: vento soprando para a direita
int wind_dy = 0;  // Exemplo: vento soprando para a direita

// Função para inicializar a floresta
void inicializar_floresta() {
    // Inicializa todas as células como livres
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            floresta[i][j].estado = '-';
            pthread_mutex_init(&floresta[i][j].lock, NULL);
        }
    }

    // Coloca células monitoradas por sensores ('T') com dois espaços livres à direita e abaixo
    for (int i = 0; i < SIZE; i += 5) {
        for (int j = 0; j < SIZE; j += 5) {
            if (i + 2 < SIZE && j + 2 < SIZE) {
                floresta[i][j].estado = 'T';
            }
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
    ThreadArgs* pos = (ThreadArgs*)arg;
    int x = pos->x;
    int y = pos->y;
    free(pos);

    while (1) {
        pthread_mutex_lock(&floresta[x][y].lock);
        if (floresta[x][y].estado == '@') {
            comunicar_incendio(x, y);
            floresta[x][y].estado = '/';  // Marca como célula queimada
            imprimir_floresta();  // Atualiza a floresta
        }
        pthread_mutex_unlock(&floresta[x][y].lock);

        // Verifica células adjacentes para incêndios e combate
        if (x > 0) {
            pthread_mutex_lock(&floresta[x-1][y].lock);
            if (floresta[x-1][y].estado == '@') {
                comunicar_incendio(x-1, y);
                floresta[x-1][y].estado = '/';
                imprimir_floresta();
            }
            pthread_mutex_unlock(&floresta[x-1][y].lock);
        }
        if (x < SIZE-1) {
            pthread_mutex_lock(&floresta[x+1][y].lock);
            if (floresta[x+1][y].estado == '@') {
                comunicar_incendio(x+1, y);
                floresta[x+1][y].estado = '/';
                imprimir_floresta();
            }
            pthread_mutex_unlock(&floresta[x+1][y].lock);
        }
        if (y > 0) {
            pthread_mutex_lock(&floresta[x][y-1].lock);
            if (floresta[x][y-1].estado == '@') {
                comunicar_incendio(x, y-1);
                floresta[x][y-1].estado = '/';
                imprimir_floresta();
            }
            pthread_mutex_unlock(&floresta[x][y-1].lock);
        }
        if (y < SIZE-1) {
            pthread_mutex_lock(&floresta[x][y+1].lock);
            if (floresta[x][y+1].estado == '@') {
                comunicar_incendio(x, y+1);
                floresta[x][y+1].estado = '/';
                imprimir_floresta();
            }
            pthread_mutex_unlock(&floresta[x][y+1].lock);
        }

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
        if (floresta[x][y].estado == '-') {
            floresta[x][y].estado = '@';  // Coloca fogo na célula
            printf("Incêndio iniciado em [%d, %d]\n", x, y);
            imprimir_floresta();  // Imprime a matriz da floresta atualizada
        }
        pthread_mutex_unlock(&floresta[x][y].lock);

        sleep(10);  // Pausa de 10 segundos entre incêndios
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
                    // Propaga o incêndio aleatoriamente para uma ou duas células vizinhas
                    int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
                    int num_propagations = rand() % 2 + 1;  // Propaga para 1 ou 2 células
                    for (int k = 0; k < num_propagations; k++) {
                        int dir_index = rand() % 4;
                        int new_i = i + directions[dir_index][0];
                        int new_j = j + directions[dir_index][1];
                        if (new_i >= 0 && new_i < SIZE && new_j >= 0 && new_j < SIZE && floresta[new_i][new_j].estado == '-') {
                            floresta[new_i][new_j].estado = '@';
                        }
                    }
                }
                pthread_mutex_unlock(&floresta[i][j].lock);
            }
        }
        imprimir_floresta();  // Atualiza a floresta
        sleep(5);  // Pausa de 5 segundos entre cada iteração de gerenciamento
    }
    return NULL;
}

// Função que gerencia o combate aos incêndios (propagação do combate)
void* combate_thread(void* arg) {
    while (1) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                pthread_mutex_lock(&floresta[i][j].lock);
                if (floresta[i][j].estado == '/') {
                    // Propaga o combate para as células vizinhas
                    if (i > 0 && floresta[i-1][j].estado == '@') floresta[i-1][j].estado = '/';
                    if (i < SIZE-1 && floresta[i+1][j].estado == '@') floresta[i+1][j].estado = '/';
                    if (j > 0 && floresta[i][j-1].estado == '@') floresta[i][j-1].estado = '/';
                    if (j < SIZE-1 && floresta[i][j+1].estado == '@') floresta[i][j+1].estado = '/';
                }
                pthread_mutex_unlock(&floresta[i][j].lock);
            }
        }
        imprimir_floresta();  // Atualiza a floresta
        sleep(2);  // Pausa de 2 segundos entre cada iteração de combate
    }
    return NULL;
}