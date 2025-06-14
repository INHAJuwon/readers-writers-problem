#include<pthread.h>
#include<stdio.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdbool.h>

sem_t x,wsem;
int readcount=0;

void* reader(void* arg){
	while(true){
		sem_wait(&x);
		readcount++;
		if(readcount==1)
			sem_wait(&wsem);
		sem_post(&x);
		printf("read\n");
		sem_wait(&x);
		readcount--;
		if(readcount==0)
			sem_post(&wsem);
		sem_post(&x);
	}
	return NULL;
}

void* writer(void* arg){
	while(true){
		sem_wait(&wsem);
		printf("write\n");
		sem_post(&wsem);
	}
}

void main(){
	pthread_t readers[2], writers[2];

	sem_init(&x,0,1);
	sem_init(&wsem,0,1);

	for(int i=0;i<2;i++){
		pthread_create(&readers[i],NULL,reader,NULL);
		pthread_create(&writers[i],NULL,writer,NULL);
	}

	for(int i=0;i<2;i++){
		pthread_join(readers[i],NULL);
		pthread_join(writers[i],NULL);
	}
}
