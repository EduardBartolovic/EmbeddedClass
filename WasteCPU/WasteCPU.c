/*
 ============================================================================
 Name        : Aufgabe3.c
 Author      : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <pthread.h>
#include <limits.h>
//#include "ec_rtutil.h"


#define PRIRORITY 30

int main(void) {

	struct timespec start_time;
	struct timespec end_time;

	/*
	set_sched_properties(SCHED_FIFO, PRIRORITY);
	int which = PRIO_PROCESS;
	id_t pid;
	int priority = 30;
	int ret;

	pid = getpid();
	ret = setpriority(which, pid, priority);*/

	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}

	unsigned int wait = 100;
	double corr = 1.0;
	waste_msecs(wait, corr);

	if (-1 == clock_gettime(CLOCK_MONOTONIC, &end_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}

	long int sec_diff = end_time.tv_sec - start_time.tv_sec;
	printf(" Sec: %ld \n", sec_diff);
	long int nsec_diff = 0;
	if (sec_diff > 0){
		nsec_diff = end_time.tv_nsec - start_time.tv_nsec + 1000000000;
		printf(" 1 Nano Sec: %ld \n", nsec_diff);
	}else{
		nsec_diff = end_time.tv_nsec - start_time.tv_nsec;
		printf(" 2 Nano Sec: %ld \n", nsec_diff);
	}

	float msec_diff = nsec_diff / 1000000;

	printf("Milli Sec: %lf \n", msec_diff);

	corr = wait / msec_diff;

	printf("Corr: %lf \n", corr);


	//++++++++++++++++++++++++++++++++++++++++++


	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}

	waste_msecs(wait,corr);

	if (-1 == clock_gettime(CLOCK_MONOTONIC, &end_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}

	sec_diff = end_time.tv_sec - start_time.tv_sec;
	nsec_diff = end_time.tv_nsec - start_time.tv_nsec;
	if (sec_diff > 0){
		nsec_diff += 1000000000;
		printf("Nano Sec: %ld \n", nsec_diff);
	}else{
		printf("Nano Sec: %ld \n", nsec_diff);
	}

	return EXIT_SUCCESS;
}

void waste_msecs (unsigned int msecs, double corr){
	int delay = 10000000;
	if (corr != 1.0){
		delay = (int) (10000000 * corr);
	}
	volatile int dummy;
	for(int counter = 0; counter < delay ;counter++){
		dummy++;
	}
}
