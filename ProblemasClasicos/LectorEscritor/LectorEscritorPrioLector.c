#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define lec 5
#define esc 1

sem_t lectores, escritores;
pthread_mutex_t  mutexLectores, mutexEscritores;
int cantLectores=0;

void* lector(){
	while(1){
        sleep(3);
        //Seccion entrada
        pthread_mutex_lock(&mutexLectores);
        //lectores comienza en 0 
		if(sem_trywait(&lectores) == 0){
            //No soy el primero 
            //Dejo el semaforo como estaba
            sem_post(&lectores);
        }else{
            //Soy el primero
            //Reservo el recurso
            pthread_mutex_lock(&mutexEscritores);
        }

        //aca cuento 1 lector
        cantLectores++;
        sem_post(&lectores);
        pthread_mutex_unlock(&mutexLectores);


        //Los lectores pueden leer de simultaneo
        printf("Lectores %i: Leo el libro\n",(pthread_self()%100), cantLectores);
        usleep(300000);
        //seccion de salida
        pthread_mutex_lock(&mutexLectores);
        //descuento lector 
        sem_wait(&lectores);
        cantLectores--;
        if(sem_trywait(&lectores) == 0){
            //No soy el ultimo 
            sem_post(&lectores);
        }else{
            //Soy el ultimo
            //Libero el recurso
            pthread_mutex_unlock(&mutexEscritores);
        }
        printf("Hay %i wachines leyendo\n", cantLectores);
        pthread_mutex_unlock(&mutexLectores);
	}
}
void* escritor(){
	while(1){
        sleep(3);
        sem_wait(&escritores);
        printf("Escritor %i: estoy esperando\n", (pthread_self()%100));
        pthread_mutex_lock(&mutexEscritores);
        //Seccion critica
		printf("Escritor %i: estoy escribiendo\n", (pthread_self()%100));
        usleep(300000);
        
        pthread_mutex_unlock(&mutexEscritores);
        sem_post(&escritores);
	}
}


int main(int argc, char **argv){
		
	pthread_t lectoresThread[lec];
	pthread_t escritoresThread[esc];

	sem_init(&lectores,0,0);
	sem_init(&escritores,0,1);
	pthread_mutex_init(&mutexLectores, NULL);
	pthread_mutex_init(&mutexEscritores, NULL);

    //Lectores
    for(int i=0;i<lec;i++){
	    pthread_create(&lectoresThread[i],NULL, &lector, NULL);
    }

	//Escritores
    for(int i=0;i<esc;i++){
	    pthread_create(&escritoresThread[i],NULL, &escritor, NULL);
    }

    //Lectores
    for(int i=0;i<lec;i++){
	    pthread_join(lectoresThread[i],NULL);
    }

	//Escritores
    for(int i=0;i<esc;i++){
	    pthread_join(escritoresThread[i],NULL);
    }
	
	sem_destroy(&lectores);
	sem_destroy(&escritores);
    pthread_mutex_destroy(&mutexEscritores);
    pthread_mutex_destroy(&mutexLectores);
	return 0;
}

