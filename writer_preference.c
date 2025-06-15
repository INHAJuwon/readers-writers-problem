#include<pthread.h>
#include<stdio.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdbool.h>

sem_t x,wsem,y,z,rsem; // 모두 binary semaphore
int readcount=0, writecount=0;

void* writer(void* arg){
	while(true){
		sem_wait(&y); // writecount 상호 배제를 위한 semaphore lock
		writecount++;
		if(writecount==1) // 가장 먼저 들어온 writer가 lock
			sem_wait(&rsem);
		sem_post(&y); // unlock
		sem_wait(&wsem);
		printf("write\n");
		sem_post(&wsem);
		sem_wait(&y); // writecount 상호 배제를 위한 semaphore lock
		writecount--;
		if(writecount==0)
			sem_post(&rsem); // 가장 마지막으로 나가는 writer가 unlock
		sem_post(&y);
	}
	return NULL;
}

void* reader(void* arg){
	while(true){
		sem_wait(&z); //만약 R1,W1,R2순으로 들어왔다면 R1이 rsem을 wait 시키고 W1도 rsem을 wait시키지만 R1이 이미 진입했음으로 sem_post(&rsem)을 하면 writer가 lock해놓았음에도 lock이 풀리기에 z로 lock
		sem_wait(&rsem); // writer가 들어와서 lock 시켜놓으면 마지막 writer가 나갈 때까지 reader들은 wait
		sem_wait(&x); // readcount를 위한 lock
		readcount++;
		if(readcount==1) 
			sem_wait(&wsem); // 가장 먼저 들어온 reader가 lock
		sem_post(&x); // unlock
		sem_post(&rsem); 
		sem_post(&z);
		printf("read\n");
		sem_wait(&x); // readcount를 위한 lock
		readcount--;
		if(readcount==0)
			sem_post(&wsem); // 가장 마지막으로 나가는 reader가 unlock
		sem_post(&x);
	}
	return NULL;
}

int main(){
	pthread_t readers[2], writers[2]; //preference를 더 명확히 보기 위해 thread를 2개씩 설정

	sem_init(&x,0,1);
	sem_init(&wsem,0,1);
	sem_init(&y,0,1);
	sem_init(&z,0,1);
	sem_init(&rsem,0,1);
	
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
