/*
 ============================================================================
 Name        : uebung5.c
 Author      : RicoFritzsche
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timing.h"

const int THREADS = 64;
int cnt=0;
int* ma;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void dreierzaehler(int j){
	int i;
	for(i=(j*2048*1024/THREADS); i<((j+1)*2048*1024/THREADS); i++){
		if(ma[i] == 3)
			pthread_mutex_lock(&mutex);
			cnt++;
			pthread_mutex_unlock(&mutex);
	}
}

void ESAYdreierzaehler(int *ma){
	int i;
	timing_t timeESAY2, timeESAY1;

	get_time(&timeESAY1);
	for(i=0; i<2097152; i++){
		if(ma[i] == 3)
			cnt++;
	}

	get_time(&timeESAY2);
	long double difftimeESAY = timespec_diff(timeESAY1, timeESAY2);

	printf("\nZahl der Threads:\t1");
	printf("\nAnzahl der 3en:\t\t%d", cnt);
	printf("\nZeit der Suche:\t\t%Lf\n", difftimeESAY);
}

int main(void) {

	int i, j;
	timing_t time1, time2;
	pthread_t threads[THREADS];
	ma = (int *) malloc(2048*1024 * sizeof(int));

	for(i=0;i<(2048*1024); i++){
		ma[i] = i%16+1;
	}


	ESAYdreierzaehler(ma);

	//start counter
	get_time(&time1);
	for(j=0; j<THREADS; j++) {
		pthread_create(&threads[j], NULL, (void*) dreierzaehler, (void*)j);
	}
	for(j=0; j<THREADS ; j++) {
		pthread_join(threads[j], NULL);
	}

	//stop counter time
	get_time(&time2);
	long double difftime = timespec_diff(time1, time2);

	printf("\nZahl der Threads:\t%d", THREADS);
	printf("\nAnzahl der 3en:\t\t%d", cnt);
	printf("\nZeit der Suche:\t\t%Lf\n", difftime);

	return 0;
}
