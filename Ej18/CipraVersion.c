/*
    N abejas, un oso y un tarro de M porciones.

    El oso duerme
    Las abejas producen miel y la guardan en el tarro
        si el terro se llena, la ultima despierta al oso

    El oso se come el tarro y sigue durmiendo

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TIME 4
#define PORCIONES 10
#define ABEJAS 3

sem_t sem_tarro;
sem_t sem_tarro_lleno;
sem_t sem_oso;
sem_t semAbejas;
int tarro=0;
pthread_mutex_t mutex_abejas;


void* abeja() {
    while (1) {
        // sleep(TIME);
        sem_wait(&semAbejas);
        // pthread_mutex_lock(&mutex_abejas);
        sem_wait(&sem_tarro);
        tarro++;
        if (sem_trywait(&sem_tarro) == 0) {
            // no soy ultima abeja
            sem_post(&sem_tarro);
            sem_post(&semAbejas);
            // pthread_mutex_unlock(&mutex_abejas);
        } else {
            // soy ultima abeja
            sem_post(&sem_oso);
        }
        // guardo porcion miel
        printf("Abeja %i guardo miel, miel: %i\n", (pthread_self()%100),tarro);
        sem_post(&sem_tarro_lleno);
    }
}

void* oso() {
    while (1) {
        // sleep(TIME);

        sem_wait(&sem_oso);

        tarro--;
        printf("Oso come miel, miel:%i\n",tarro);
        sem_wait(&sem_tarro_lleno);
        if (sem_trywait(&sem_tarro_lleno) == 0) {
            // no es ultima porcion
            sem_post(&sem_tarro_lleno);
            sem_post(&sem_oso);
        } else {
            // es ultima porcion
            sem_post(&semAbejas);
            // pthread_mutex_unlock(&mutex_abejas);
        }
        sem_post(&sem_tarro);
    }
}

void main() {
    pthread_t abejas_t[ABEJAS];
    pthread_t oso_t;

    sem_init(&sem_tarro, 0, PORCIONES);
    sem_init(&sem_tarro_lleno, 0, 0);
    sem_init(&sem_oso, 0, 0);
    sem_init(&semAbejas, 0, 1);

    for (int i = 0; i < ABEJAS; i++) {
        pthread_create(&abejas_t[i], NULL, &abeja, NULL);
    }

    pthread_create(&oso_t, NULL, &oso, NULL);

    pthread_join(oso_t, NULL);

    for (int i = 0; i < ABEJAS; i++) {
        pthread_join(abejas_t[i], NULL);
    }

    sem_destroy(&sem_tarro);
    sem_destroy(&semAbejas);
    sem_destroy(&sem_tarro_lleno);
    sem_destroy(&sem_oso);
}