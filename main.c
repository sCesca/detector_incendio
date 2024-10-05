#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

#define NUM_THREADS 10  // Número fixo de threads de sensores

typedef struct {
    int id;
    int num_threads;
} ThreadArgs;

void* sensor_thread(void* arg);  // Declaração da função sensor_thread

int main() {
    // Inicializa a floresta
    inicializar_floresta();
    imprimir_floresta();

    // Cria um número fixo de threads de sensores
    pthread_t sensores[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        ThreadArgs* args = malloc(sizeof(ThreadArgs));
        args->id = i;
        args->num_threads = NUM_THREADS;
        if (pthread_create(&sensores[i], NULL, sensor_thread, args) != 0) {
            perror("Failed to create sensor thread");
            return 1;
        }
        printf("Sensor thread %d created\n", i);
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

    // Aguarda a conclusão das threads (neste caso, elas nunca terminam)
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(sensores[i], NULL);
    }
    pthread_join(thread_incendios, NULL);
    pthread_join(thread_central, NULL);

    return 0;
}