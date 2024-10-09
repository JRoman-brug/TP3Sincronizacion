#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 20
int buffer[N];
int i=0;

sem_t vacio, lleno;
pthread_mutex_t  mutexBuffer;

void* productor(){
	int item;
	while(1){
		item = rand() % 100;
		sem_wait(&vacio);
		pthread_mutex_lock(&mutexBuffer);
		buffer[i]=item;
		i++;
		printf("Productor: agrego un producto:  %i\n", item);
		usleep(100000);
		pthread_mutex_unlock(&mutexBuffer);
		sem_post(&lleno);
	}
}
void* consumidor(){
	int item;
	while(1){
		sem_wait(&lleno);
		pthread_mutex_lock(&mutexBuffer);
		i--;
		item = buffer[i];
		buffer[i]=0;
		printf("Consumidor: saco un producto: %i\n", item);
		usleep(100000);
		pthread_mutex_unlock(&mutexBuffer);
		sem_post(&vacio);
	}
}


int main(int argc, char **argv){
		
	pthread_t thread[2];
	
	sem_init(&vacio,0,N);
	sem_init(&lleno,0,0);
	pthread_mutex_init(&mutexBuffer, NULL);

	pthread_create(&thread[0],NULL, &productor, NULL);
	pthread_create(&thread[1],NULL, &consumidor, NULL);
	
	pthread_join(thread[0],NULL);
	pthread_join(thread[1],NULL);
	
	
	sem_destroy(&vacio);
	sem_destroy(&lleno);
    pthread_mutex_destroy(&mutexBuffer);
	return 0;
}

