#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define cantMujer 4
#define cantHombre 4
#define capSalon 3
#define cortePelo 300000

/*
entran varios hombres y mujeres
2 condiciones
hombres y mujeres no puede estar juntos
no mas de 3 personas


si(soy primer hombre)
	bloqueo mujer

sumo un lugar
//Hago algo xd
libero un lugar

si (soy el ultimo hombre)
	desbloqueo mujer
*/
sem_t semHombres, semCantHombres,  semCantMujeres, semMujeres;
pthread_mutex_t  mutexHombres, mutexMujeres;
int contMujer = 0;
int contHombre = 0;
void* mujer(){
	while(1){

		//Preguntar por la condicion de carrera en este caso
		//Preguntar si esta bien el primer semaforo, porque al tener separados los semaforos 
		//seccion de entrada
		//descuento un lugar en el lugar
		sem_wait(&semMujeres);
		pthread_mutex_lock(&mutexMujeres);
		if(sem_trywait(&semCantMujeres) == 0){
			//No soy la primer mujer
			//dejo el semaforo como estaba 
			sem_post(&semCantMujeres);
		}else{
			//soy la primer mujer
			//bloquear el recurso a los hombres
			pthread_mutex_lock(&mutexHombres);
		}	
		//cuento una mujer 
		sem_post(&semCantMujeres);
		contMujer++;
		pthread_mutex_unlock(&mutexMujeres);
		
		//Accion
		printf("Mujer %i: se corta el pelo\n",contMujer);
		usleep(cortePelo);

		//seccion de salida
		pthread_mutex_lock(&mutexMujeres);
		//descuento una mujer
		sem_post(&semCantMujeres);
		contMujer--;
		if(sem_trywait(&semCantMujeres) == 0){
			//No soy la primer mujer
			//dejo el semaforo como estaba
			sem_post(&semCantMujeres);
		}else{
			//soy la ultima mujer
			//libero el recurso
			pthread_mutex_unlock(&mutexHombres);
		}
		pthread_mutex_unlock(&mutexMujeres);
		//sumo un lugar en el salon
		sem_post(&semMujeres);
	}
}
void* hombre(){
	while(1){
		//seccion de entrada
		//descuento un lugar en el lugar
		sem_wait(&semHombres);
		pthread_mutex_lock(&mutexHombres);
		if(sem_trywait(&semCantHombres) == 0){
			//No soy el primer hombre
			//dejo el semaforo como estaba 
			sem_post(&semCantHombres);
		}else{
			//soy el primer hombre
			//bloquear el recurso a las mujeres
			pthread_mutex_lock(&mutexMujeres);
		}	
		//cuento una mujer 
		sem_post(&semCantHombres);
		contHombre++;
		pthread_mutex_unlock(&mutexHombres);
		
		//Accion
		printf("Hombre %i: se corta el pelo\n",contHombre);
		usleep(cortePelo);


		//seccion de salida
		pthread_mutex_lock(&mutexHombres);
		//descuento un hombre
		sem_post(&semCantHombres);
		contHombre--;
		if(sem_trywait(&semCantHombres) == 0){
			//No soy el primer hombre
			//dejo el semaforo como estaba
			sem_post(&semCantHombres);
		}else{
			//soy la ultimo hombre
			//libero el recurso
			pthread_mutex_unlock(&mutexMujeres);
		}
		pthread_mutex_unlock(&mutexHombres);
		//sumo un lugar en el salon
		sem_post(&semHombres);
	}
}


int main(int argc, char **argv){
	pthread_t mujeres[cantMujer];
	pthread_t hombres[cantHombre];

	//contadores de cantidad de personas en el salon
	sem_init(&semMujeres,0,capSalon);
	sem_init(&semHombres,0,capSalon);
	//Contadores de lugares disponibles
	sem_init(&semCantMujeres,0,0);
	sem_init(&semCantHombres,0,0);
	//Mutexs 
	pthread_mutex_init(&mutexMujeres, NULL);
	pthread_mutex_init(&mutexHombres, NULL);

	for (int i = 0; i < cantMujer; i++){
		pthread_create(&mujeres[i],NULL, &mujer, NULL);
	}
	for (int i = 0; i < cantHombre; i++){
		pthread_create(&hombres[i],NULL, &hombre, NULL);
	}
	
	for (int i = 0; i < cantMujer; i++){
		pthread_join(mujeres[i],NULL);
	}
	for (int i = 0; i < cantHombre; i++){
		pthread_join(hombres[i],NULL);
	}
	
	
	sem_destroy(&semMujeres);
	sem_destroy(&semHombres);
	sem_destroy(&semCantMujeres);
	sem_destroy(&semCantHombres);
    pthread_mutex_destroy(&mutexMujeres);
    pthread_mutex_destroy(&mutexHombres);
	return 0;
}

