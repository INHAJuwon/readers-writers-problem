#include<pthread.h>
#include<stdio.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdbool.h>

sem_t x,wsem; //x는 상호배제를 위한 binary semaphore, wsem도 상호배제를 위한 binary semaphore지만 특성상 writer에게 불리하게 작용
int readcount=0;

void* reader(void* arg){
	while(true){
		sem_wait(&x); // 전역변수 readcount를 위한 상호배제
		readcount++;
		if(readcount==1) // 가장 먼저 들어오는 reader가 readcount를 ++하면서 wsem을 lock
			sem_wait(&wsem);
		sem_post(&x); // unlock
		printf("read\n");
		sem_wait(&x); // 전역변수 readcount를 위한 상호배제
		readcount--;
		if(readcount==0)
			sem_post(&wsem); // 가장 마지막으로 나가는 reader가 wsem을 unlock -> 가장 처음과 가장 마지막 reader동안 writer는 계속 wait
		sem_post(&x); // unlock
	}
	return NULL;
}

void* writer(void* arg){
	while(true){
		sem_wait(&wsem); //reader들이 없는 경우가 되어서야 lock하고 실행 가능
		printf("write\n");
		sem_post(&wsem); // unlock
	}
	return NULL;
}

int main(){
	pthread_t readers[2], writers[2]; // reader와 writer사이에서 preference가 어디에 치중되어있는지 더 극명한 차이를 보기 위해 thread를 각 2개씩 생성

	sem_init(&x,0,1); // binary semaphore
	sem_init(&wsem,0,1); // binary semaphore

	for(int i=0;i<2;i++){
		pthread_create(&readers[i],NULL,reader,NULL);
		pthread_create(&writers[i],NULL,writer,NULL);
	}

	for(int i=0;i<2;i++){
		pthread_join(readers[i],NULL);
		pthread_join(writers[i],NULL);
	}
	return 0;
}
