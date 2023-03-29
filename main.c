
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "ts_hashmap.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: %s <num threads> <hashmap capacity>\n", argv[0]);
		return 1;
	}
	if(atoi(argv[1])<1||atoi(argv[2])<1){
		printf("Please make sure both arguments are greater than 0");
		return 1;
	}
	

	int num_threads = atoi(argv[1]);
	int capacity = (unsigned int) atoi(argv[2]);
	//test
	ts_hashmap_t* threadedHashmap = initmap(capacity);
	srand(69); //nice
	pthread_t* tid = (pthread_t*)malloc(num_threads*sizeof(pthread_t));
	for(int i = 0; i<num_threads;i++){
		threadArgs* args = (threadArgs*)malloc(sizeof(threadArgs));
		args->currthread = i;
		args->numThreads = num_threads;
		args->map = threadedHashmap;
		pthread_create(&tid[i],NULL,parTest,args);
	}
	for(int i = 0;i<num_threads;i++){
		pthread_join(tid[i],NULL);
	}
	free(tid);
	printmap(threadedHashmap);
	freemap(threadedHashmap);
	ts_hashmap_t* seqHashmap = initmap(capacity);
	srand(69);//nice again
	for(int i = 0;i<1000;i++){
		int random =rand();
		int command = random%3;
		int keyValue = random%100;
		if(command ==0){
			put(seqHashmap,keyValue,keyValue);
		} else if(command==1){
			del(seqHashmap,keyValue);
		} else if(command ==2){
			get(seqHashmap,keyValue);
		}
	}

	printmap(seqHashmap);
	freemap(seqHashmap);
	// TODO: Write your test

	return 0;
}

