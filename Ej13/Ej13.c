#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef enum{
    vacio = 0,
    tabaco = 1,
    papel = 2,
    fosforos = 3
}Materiales;

sem_t semAgente;
pthread_mutex_t fumador, materiales;
int materialesAgente[2] = {vacio,vacio};

void* fumadorTabaco() {
    while(1) {
        pthread_mutex_lock(&fumador);
        if( (materialesAgente[0]==papel && materialesAgente[1]==fosforos) || 
            (materialesAgente[0]==fosforos && materialesAgente[1]==papel)
        ){
            pthread_mutex_lock(&materiales);
            printf("Fumador tabaco: Arma el cigarrillo y lo fuma\n");
            materialesAgente[0] =vacio;
            materialesAgente[1] =vacio;
            sem_post(&semAgente);
            pthread_mutex_unlock(&materiales);
        }
        pthread_mutex_unlock(&fumador);
    }
}

void* fumadorPapel() {
    while(1) {
        pthread_mutex_lock(&fumador);
        if( (materialesAgente[0]==tabaco && materialesAgente[1]==fosforos) || 
            (materialesAgente[0]==fosforos && materialesAgente[1]==tabaco)
        ){
            pthread_mutex_lock(&materiales);
            printf("Fumador papel: Arma el cigarrillo y lo fuma\n");
            materialesAgente[0] =vacio;
            materialesAgente[1] =vacio;
            sem_post(&semAgente);
            pthread_mutex_unlock(&materiales);
        }
        pthread_mutex_unlock(&fumador);

        
    }
}

void* fumadorFosforo() {
    while(1) {
        if( (materialesAgente[0]==papel && materialesAgente[1]==tabaco) || 
            (materialesAgente[0]==tabaco && materialesAgente[1]==papel)
        ){ 
            //reserva el recurso
            pthread_mutex_lock(&fumador);
            pthread_mutex_lock(&materiales);
        }
        printf("Fumador fosforo: Arma el cigarrillo y lo fuma\n");
        materialesAgente[0] =vacio;
        materialesAgente[1] =vacio;
        sem_post(&semAgente);
        pthread_mutex_unlock(&materiales);
        pthread_mutex_unlock(&fumador);

        
    }
}

void* agente() {
    while(1) {
        sem_wait(&semAgente);
        int mat;

        int mat3 [3] = {tabaco, papel, fosforos};
        mat = (rand()%3)+1;
        materialesAgente[0] = mat3[mat];
        
        int mat2[2];
        int j=0;
        for(int i=0;i<3;i++){
            if(materialesAgente[0] != mat3[i]) mat2[j++] = mat3[i];
        }
        mat = (rand()%2)+1;
        materialesAgente[1] = mat2[mat];
        printf("El agente puso los materiales en la mesa: %i - %i \n",materialesAgente[0],materialesAgente[1]);
        usleep(100000);
        
    }
}
int main() {
    pthread_t threadA, threadB, threadC;

    sem_init(&semAgente,0,1);
    pthread_mutex_init(&fumador, NULL);
    pthread_mutex_init(&materiales, NULL);

    pthread_create(&threadA, NULL, fumadorTabaco, NULL);
    pthread_create(&threadB, NULL, fumadorPapel, NULL);
    pthread_create(&threadC, NULL, fumadorFosforo, NULL);
    pthread_create(&threadC, NULL, agente, NULL);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);
    pthread_join(threadC, NULL);

    sem_close(&semAgente);
    sem_destroy(&semAgente);
    pthread_mutex_destroy(&fumador);
    pthread_mutex_destroy(&materiales);
    return 0;
}