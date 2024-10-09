#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define cantAbejas 2
#define tarroSize 20

sem_t semOso, semAbeja, vacio, lleno, semTermino;
pthread_mutex_t abejas;
int tarro=0;
void* abeja(){
    while(1){
        //seccion de entrada
        pthread_mutex_lock(&abejas);
        sem_wait(&vacio);
        
        tarro++;
        // usleep(100000);
        printf("Abeja %i: tarro miel: %i\n", (pthread_self()%100), tarro);

        //Aviso que se agrego 1 de miel
        sem_post(&lleno);
        pthread_mutex_unlock(&abejas);

        pthread_mutex_lock(&abejas);
        //tengo que ver si se lleno -> el oso empieza consumir
        if(sem_trywait(&vacio) == -1){
            //esta completamente lleno -> despierto al oso
            sem_post(&semOso);
            printf("Abeja despierta oso\n");
            sem_wait(&semTermino);
        }else{
            //dejo intacto el semaforo y pueden seguir produciendo 
            // sem_post(&semAbeja);
            sem_post(&vacio);
        }
        pthread_mutex_unlock(&abejas);
    }
}

void* oso(){
    while(1){
        
        sem_wait(&semOso);
        printf("Oso despierto\n");
        //Preguntar por un mutex si necesito un mutex para cada entidad (oso y abejas) o uno compartido
        
        // usleep(100000);
        for (int i = 0; i < tarroSize; i++){
            tarro--;
            sem_post(&vacio);
            sem_wait(&lleno);
            printf("El oso: come, tarro miel: %i\n", tarro);
        }
        
        printf("Oso: Oso se durmio\n");
        sem_post(&semTermino);
    }
}


int main() {
    pthread_t threadAbejas[cantAbejas];
    pthread_t threadOso;
    sem_init(&vacio,0,tarroSize);
    sem_init(&lleno,0,0);
    sem_init(&semOso,0,0);
    sem_init(&semTermino,0,0);
    pthread_mutex_init(&abejas, NULL);

    for(int i=0;i<cantAbejas;i++){
        pthread_create(&threadAbejas[i],NULL, &abeja, NULL);
    }
    pthread_create(&threadOso, NULL, &oso, NULL);

    for(int i=0;i<cantAbejas;i++){
        pthread_join(threadAbejas[i],NULL);
    }
    pthread_join(threadOso,NULL);
    

    sem_destroy(&vacio);
    sem_destroy(&lleno);
    sem_destroy(&semOso);
    sem_destroy(&semTermino);
    pthread_mutex_destroy(&abejas);

}