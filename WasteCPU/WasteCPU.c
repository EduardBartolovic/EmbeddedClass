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


#define PRIRORITY 30

void waste_msecs(unsigned int msecs, double corr);

double calculateDiff(bool prints, struct timespec start_time, struct timespec end_time);

double makeRun(bool prints, int wait, double corr,  struct timespec start_time, struct timespec end_time);

void* threadFunction( void* arg);

int main(void) {
    
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, threadFunction, NULL);
    
    //struct sched_param{
    //    50;//sched priority
    //}

    //error handling
    //pthread_attr_getschedparam(pthread_attr_t *attr, struct sched_param *param);
    
	return EXIT_SUCCESS;
}


void* threadFunction( void* arg){
    
    struct timespec start_time;
	struct timespec end_time;
	double msec_diff;
	bool printing;
	double corr;
	unsigned int wait;
	
	wait = 150;
	corr = 1.0;
	printing = true;

	makeRun(printing, wait, corr, start_time, end_time );

	//+++++++++++++++++++++++

	msec_diff = makeRun(printing, wait, corr, start_time, end_time );

	//+++++++++++++++++++++++

	corr = wait / msec_diff;
	printf("\nCalibration Ready => Corr: %lf \n", corr);

	//+++++++++++++++++++++++

	for (int counter = 0; counter < 15; counter++){
		msec_diff = makeRun(printing, wait, corr, start_time, end_time );
	}

	return 0;
}

double makeRun(bool prints, int wait, double corr, struct timespec start_time, struct timespec end_time){
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}
	waste_msecs(wait, corr);
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &end_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}
	double msec_diff = calculateDiff(prints, start_time, end_time);
	return msec_diff;
}

void waste_msecs(unsigned int msecs, double corr){
	int delay = 100000* msecs;
	if (corr != 1.0){
		delay = (int) (100000 * corr * msecs);
	}
	volatile int dummy;
	for(int counter = 0; counter < delay ;counter++){
		dummy++;
	}
}

double calculateDiff(bool prints, struct timespec start_time, struct timespec end_time){
	long int sec_diff = end_time.tv_sec - start_time.tv_sec;
	if(prints) {printf(" Sec: %ld \n", sec_diff);}
	long int nsec_diff = 0;
	if (sec_diff > 0){
		nsec_diff = end_time.tv_nsec - start_time.tv_nsec + 1000000000;
		if(prints) {printf(" 1 Nano Sec: %ld \n", nsec_diff);}
	}else{
		nsec_diff = end_time.tv_nsec - start_time.tv_nsec;
		if(prints) {printf(" 2 Nano Sec: %ld \n", nsec_diff);}
	}

	double msec_diff = (double)nsec_diff / 1000000;
	printf("Milli Sec: %lf \n", msec_diff);
	return msec_diff;
}
