#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TIME 3
#define LECTORES 3
#define ESCRITORES 2


sem_t sem_lectores;
sem_t sem_escritores;

pthread_mutex_t mutex_lectores;
pthread_mutex_t mutex_file;

void* lector() {
    while (1) {
        // sleep(TIME);
        pthread_mutex_lock(&mutex_lectores);
        if (sem_trywait(&sem_lectores) == -1) {  
            // soy el primero
            pthread_mutex_lock(&mutex_file); //bloqueo el acceso al archivo
        } else { 
            // no soy el primero
            sem_post(&sem_lectores);
        }
        sem_post(&sem_lectores);
        pthread_mutex_unlock(&mutex_lectores);
        
        
        // leer

        printf("Lector %i: leo el libro\n",(pthread_self()%100));

        pthread_mutex_lock(&mutex_lectores);
        sem_wait(&sem_lectores);
        if (sem_trywait(&sem_lectores) == -1) { 
            //soy el ultimo
            pthread_mutex_unlock(&mutex_file); //libero el acceso al archivo
        } else { 
            //no soy el ultimo
            sem_post(&sem_lectores);
        }
        pthread_mutex_unlock(&mutex_lectores);
    }

}

void* escritor() {
    while (1) {
        // sleep(TIME);
        sem_wait(&sem_escritores);
        printf("Escritor espera\n");
        pthread_mutex_lock(&mutex_file);
        
        // escribir
        printf("Escritor %i: Escribo el libro\n", (pthread_self()%100));

        pthread_mutex_unlock(&mutex_file);
        sem_post(&sem_escritores);
    }
}

void main() {
    pthread_t lecto_t[LECTORES];
    pthread_t escritor_t[ESCRITORES];

    sem_init(&sem_lectores, 0, 0);
    sem_init(&sem_escritores, 0, 1);

    // pthread create
    
    for (int i = 0; i < LECTORES; i++) {
        pthread_create(&lecto_t[i], NULL, lector, NULL);
    }

    for (int i = 0; i < ESCRITORES; i++) {
        pthread_create(&escritor_t[i], NULL, escritor, NULL);
    }

    // pthread join

    for (int i = 0; i < ESCRITORES; i++) {
        pthread_join(escritor_t[i], NULL);
    }
    for (int i = 0; i < LECTORES; i++) {
        pthread_join(lecto_t[i], NULL);
    }

    sem_close(&sem_lectores);
    sem_close(&sem_escritores);
}