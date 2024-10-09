#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

//Secuencia (A o B)CDE (A o B)CDE (A o B)CDE...
sem_t semAoB,semC,semD,semE;

void* printA(){
	while(1){		
		sem_wait(&semAoB);
		printf("A");
		usleep(100000);
		sem_post(&semC);
	}
}

void* printB(){
	while(1){		
		sem_wait(&semAoB);
		printf("B");
		usleep(100000);
		sem_post(&semC);
	}
}

void* printC(){
	while(1){		
		sem_wait(&semC);
		printf("C");
		usleep(100000);
		sem_post(&semD);
	}
}

void* printD(){
	while(1){		
		sem_wait(&semD);
		printf("D");
		usleep(100000);
		sem_post(&semE);
	}
}

void* printE(){
	while(1){		
		sem_wait(&semE);
		printf("E\n");
		usleep(100000);
		sem_post(&semAoB);
	}
}
int main(int argc, char **argv){
		
	pthread_t thread[5];
	
	sem_init(&semAoB,0,1);
	sem_init(&semC,0,0);
	sem_init(&semD,0,0);
	sem_init(&semE,0,0);
	
	
	pthread_create(&thread[0],NULL, &printA, NULL);
	pthread_create(&thread[1],NULL, &printB, NULL);
	pthread_create(&thread[2],NULL, &printC, NULL);
	pthread_create(&thread[3],NULL, &printD, NULL);
	pthread_create(&thread[4],NULL, &printE, NULL);
	
	pthread_join(thread[0],NULL);
	pthread_join(thread[1],NULL);
	pthread_join(thread[2],NULL);
	pthread_join(thread[3],NULL);
	pthread_join(thread[4],NULL);
	
	sem_close(&semAoB);
	sem_close(&semC);
	sem_close(&semD);
	sem_close(&semE);
	
	sem_destroy(&semAoB);
	sem_destroy(&semC);
	sem_destroy(&semE);
	sem_destroy(&semD);
	
	return 0;
}

