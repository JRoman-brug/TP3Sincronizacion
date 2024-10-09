#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
 El oso y las abejas. Se tienen n abejas y un oso hambriento. Comparten un
 tarro de miel. Inicialmente el tarro de miel está vacío, su capacidad es
 M porciones de miel. El oso duerme hasta que el tarro de miel se llene,
 entonces se come toda la miel y vuelve a dormir. Cada abeja produce
 una porción de miel que coloca en el tarro, la abeja que llena el tarro
 de miel despierta al oso.
 Escriba un programa para que sincronice a las abejas y al oso.
*/

#define M 20

sem_t mielVacio, mielLleno, semOso, semTermino;
pthread_mutex_t mutex;
int contadorMiel = 0;

void *abeja(void *arg)
{
    while (1)
    {
        if (sem_trywait(&mielVacio) == 0)
        {
            pthread_mutex_lock(&mutex);
            printf("Agregando miel.\n");
            pthread_mutex_unlock(&mutex);
            // sleep(1);
            sem_post(&mielLleno);
        }
        else
        {
            sem_post(&semOso);
            sem_wait(&semTermino);
        }
    }
    return NULL;
}

void *oso(void *arg)
{
    while (1)
    {
        printf("El oso está durmiendo. Un oso wacho!\n");
        sem_wait(&semOso);
        printf("Despierta el oso! Listo para comer la miel.\n");

        for (int i = 0; i < M; i++) 
        {
            sem_wait(&mielLleno); 
            pthread_mutex_lock(&mutex);
                printf("Sacando la porción %d de miel.\n", contadorMiel + 1);
                contadorMiel++;
            pthread_mutex_unlock(&mutex);
        }

        for (int j = 0; j < M; j++)
        {
            sem_post(&mielVacio);  
        }
        printf("Se lo comió todo el oso puto.\n");
        sem_post(&semTermino); 
        contadorMiel = 0;
    }

    return NULL;
}

int main()
{
    pthread_t threadA, threadB;

    sem_init(&mielVacio, 0, M);
    sem_init(&mielLleno, 0, 0);
    sem_init(&semOso, 0, 0);
    sem_init(&semTermino, 0, 0);

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&threadA, NULL, abeja, NULL);
    pthread_create(&threadB, NULL, oso, NULL);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    sem_destroy(&mielVacio);
    sem_destroy(&mielLleno);
    sem_destroy(&semOso);
    sem_destroy(&semTermino);

    printf("\n");

    return 0;
}
