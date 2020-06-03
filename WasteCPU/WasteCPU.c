#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <pthread.h>
#include <limits.h>


void waste_msecs(unsigned int msecs, double corr);

double calculateDiff( struct timespec start_time, struct timespec end_time);

double makeRun( int wait, double corr,  struct timespec start_time, struct timespec end_time);

void* threadFunction( void* arg);

int main(void) {

	struct sched_param sh_param;
	int policy = SCHED_FIFO;
	sh_param.sched_priority = 49;
	if (-1 == sched_setscheduler(0,policy, &sh_param)){
		perror ("Error in Create");
		exit (EXIT_FAILURE);
	}

	pthread_t thread_id;
	pthread_attr_t attr;

	if (-1 == pthread_attr_init(&attr)){
		perror ("Error in Create");
		exit (EXIT_FAILURE);
	}

	if (-1 == pthread_create(&thread_id, &attr, threadFunction, NULL)){
		perror ("Error in Create");
		exit (EXIT_FAILURE);
	}

	if (-1 == pthread_join(thread_id, NULL)){
		perror ("Error in JOIN");
		exit (EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

void* threadFunction( void* arg){

    	struct timespec start_time;
	struct timespec end_time;
	int LOOPITERATIONS = 10;
	double corr;
	unsigned int wait;
	double msec_diff[LOOPITERATIONS];

	wait = 50;
	corr = 1.0;

	makeRun( wait, corr, start_time, end_time );//warm up

	//+++++++++++++++++++++++

	int rekord_index = -1;
	double rekord = -1;
	double msec_diff_run;
	for(int counter = 0 ; counter < LOOPITERATIONS; counter++){
		msec_diff_run = makeRun( wait, corr, start_time, end_time );
		if (msec_diff_run > rekord){
			rekord = msec_diff_run;
			rekord_index = counter;
		}
		msec_diff[counter] = msec_diff_run;
	}

	double totalTime = 0;
	for(int counter = 0 ; counter < LOOPITERATIONS; counter++){
		if (counter != rekord_index){ //ignore highest value
			totalTime += msec_diff[counter];
		}
	}

	corr = wait / (totalTime/(LOOPITERATIONS-1));
	printf("\nCalibration Ready => Corr: %lf \n", corr);

	//+++++++++++++++++++++++

	for (int counter = 0; counter < 20; counter++){
		makeRun( wait, corr, start_time, end_time );
		//sleep(1);
	}

	return 0;
}

double makeRun( int wait, double corr, struct timespec start_time, struct timespec end_time){
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}
	waste_msecs(wait, corr);
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &end_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}
	double msec_diff = calculateDiff( start_time, end_time);
	return msec_diff;
}

void waste_msecs(unsigned int msecs, double corr){
	int delay = (int) (100000 * msecs * corr);
	volatile int dummy;
	for(int counter = 0; counter < delay ;counter++){
		dummy++;
	}
}

double calculateDiff( struct timespec start_time, struct timespec end_time){
	long int sec_diff = end_time.tv_sec - start_time.tv_sec;
	//printf(" Sec: %ld \n", sec_diff);
	long int nsec_diff = 0;
	if (sec_diff > 0){
		nsec_diff = end_time.tv_nsec - start_time.tv_nsec + 1000000000;
		//printf(" 1 Nano Sec: %ld \n", nsec_diff);
	}else{
		nsec_diff = end_time.tv_nsec - start_time.tv_nsec;
		//printf(" 2 Nano Sec: %ld \n", nsec_diff);
	}

	double msec_diff = (double)nsec_diff / 1000000;
	printf("Milli Sec: %lf \n", msec_diff);
	return msec_diff;
}
