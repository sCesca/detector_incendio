#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

#define SIZE 30

int main() {
    // Inicializa a floresta
    inicializar_floresta();
    imprimir_floresta();

    // Cria threads para os sensores nas posições com 'T'
    pthread_t sensores[SIZE * SIZE];
    int thread_count = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (floresta[i][j].estado == 'T') {
                ThreadArgs* args = malloc(sizeof(ThreadArgs));
                args->x = i;
                args->y = j;
                if (pthread_create(&sensores[thread_count], NULL, sensor_thread, args) != 0) {
                    perror("Failed to create sensor thread");
                    return 1;
                }
                printf("Sensor thread created for position [%d, %d]\n", i, j);
                thread_count++;
            }
        }
    }

    // Cria a thread para gerar incêndios
    pthread_t thread_incendios;
    if (pthread_create(&thread_incendios, NULL, gerar_incendios, NULL) != 0) {
        perror("Failed to create fire generation thread");
        return 1;
    }
    printf("Fire generation thread created\n");

    // Cria a thread central para gerenciar o combate aos incêndios
    pthread_t thread_central;
    if (pthread_create(&thread_central, NULL, central_thread, NULL) != 0) {
        perror("Failed to create central thread");
        return 1;
    }
    printf("Central thread created\n");

    // Cria a thread para propagar o combate aos incêndios
    pthread_t thread_combate;
    if (pthread_create(&thread_combate, NULL, combate_thread, NULL) != 0) {
        perror("Failed to create combat propagation thread");
        return 1;
    }
    printf("Combat propagation thread created\n");

    // Aguarda a conclusão das threads de sensores (neste caso, elas nunca terminam)
    for (int i = 0; i < thread_count; i++) {
        pthread_join(sensores[i], NULL);
    }
    pthread_join(thread_incendios, NULL);
    pthread_join(thread_central, NULL);
    pthread_join(thread_combate, NULL);

    return 0;
}