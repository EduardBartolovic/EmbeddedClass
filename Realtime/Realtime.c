
/*
 ============================================================================
 Name        : Aufgabe4.c
 Author      : Eduard Bartolovic, Richard Reik
 Version     :
 Copyright   : This is ours, dont touch this
 Description : Hello World in C, Ansi-style
 ============================================================================





 Todo:
 	 - Reaktionszeit des Systems bestimmen -> fragen was genau gemeint ist
 	 - Semaphore einfügen
 	 - Threads mit Semaphorenlogik verbinden

 	 - Dokumentieren

 Fragen:
 	 - Echtzeit bei 3 ms eingehalten?



 */

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

double corr_factor = 1.0;

void waste_msecs(unsigned int msecs);

double calculateDiff( struct timespec start_time, struct timespec end_time);

double makeRun( int wait,  struct timespec start_time, struct timespec end_time);

void* threadFunction( void* arg); //höhere Priorität(da
void* threadFunctionZwei( void* arg); //niedrigere Deadline

void prepEverything();

int main(void) {
	unsigned int wait = 2;

	struct sched_param sh_param;
	int policy = SCHED_FIFO;
	sh_param.sched_priority = 1;
	if (-1 == sched_setscheduler(0,policy, &sh_param)){
		perror ("Error in Create");
		exit (EXIT_FAILURE);
	}
	prepEverything(wait); //in ms um den Corr faktor anzupassen

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

	/* if (-1 == pthread_create(&thread_id, &attr, threadFunctionZwei, NULL)){
		perror ("Error in Create");
		exit (EXIT_FAILURE);
	} */

	if (-1 == pthread_join(thread_id, NULL)){
		perror ("Error in JOIN");
		exit (EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

void prepEverything(unsigned int wait){
	//Estimate optimal corr_factor
    struct timespec start_time;
	struct timespec end_time;
	int LOOPITERATIONS = 10;

	wait = 3;
	corr_factor = 1.0;

	makeRun( wait, start_time, end_time );//warm up

	//+++++++++++++++++++++++

	double rekord = 10000000;
	double msec_diff_run;
	for(int counter = 0 ; counter < LOOPITERATIONS; counter++){
		msec_diff_run = makeRun( wait, start_time, end_time );
		if (msec_diff_run < rekord){
			rekord = msec_diff_run;
		}
		//sleep(1);
	}

	corr_factor = wait / rekord;
	printf("\nCalibration Ready => Corr: %lf \n", corr_factor);

	return;
}

void* threadFunction( void* arg){
	int long LOOP_ITERATIONS = 12;
	struct timespec end_time;	// Endzeit
	struct timespec start_time;
	struct timespec validate_time;
	int long WRAP_AROUNT = 996000000;
	int long MIO = 4000000;

	long int validate_nsec[LOOP_ITERATIONS]; //validiere die verstrichenen NS
	long int validate_sec[LOOP_ITERATIONS]; //validiere die verstrichenen SEc


	//Set Priority and Shedular
	struct sched_param sh_param;
	int policy = SCHED_FIFO;
	sh_param.sched_priority = 48;
	if (-1 == sched_setscheduler(0,policy, &sh_param)){
		perror ("Error in Create");
		exit (EXIT_FAILURE);
	}

	pthread_getschedparam(pthread_self(), &policy, &sh_param);
	printf("Priority=%d\n", sh_param.__sched_priority);


	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}

	//Eigentliches Programm
	for (int i = 0; i < LOOP_ITERATIONS; ++i) //Periodenschleife
		{
			if (start_time.tv_nsec >= WRAP_AROUNT ){ //Falls ein Sekunden wraparound passieren sollte.
				start_time.tv_sec += 1;
				start_time.tv_nsec = start_time.tv_nsec %  WRAP_AROUNT;
			} else {
				start_time.tv_nsec += MIO; //sonst 1ms warten
			}
			waste_msecs(2);

	    	if (-1 == clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time, NULL)){ //warten bis Zielzeit erreicht
	    		perror ("Error in WAIT");
	    		exit (EXIT_FAILURE);
	    	}

	    	if (-1 == clock_gettime(CLOCK_MONOTONIC, &validate_time)){ //Holen der Aktuellen Zeit für die Validierung
	    		perror ("Error in get Time");
	    		exit (EXIT_FAILURE);
	    	}
	    	validate_nsec[i] = validate_time.tv_nsec; //Speichern der Validierungszeiten sec
	    	validate_sec[i] = validate_time.tv_sec;//Speichern der Validierungszeiten nano sec
		}
	for (int i = 0; i < LOOP_ITERATIONS-1; ++i) //Periodenschleife
		{
			printf("Sekunde = %ld \n", validate_sec[i+1] - validate_sec[i]);
			printf("NSekunde = %ld \n\n", validate_nsec[i+1] - validate_nsec[i]);
		}

	return 0;
}


void* threadFunctionZwei( void* arg){
	//Set Priority and Shedular
	struct sched_param sh_param;
	int policy = SCHED_FIFO;
	sh_param.sched_priority = 49;
	if (-1 == sched_setscheduler(0,policy, &sh_param)){
		perror ("Error in Create");
		exit (EXIT_FAILURE);
	}

	pthread_getschedparam(pthread_self(), &policy, &sh_param);
	printf("Priority=%d\n", sh_param.__sched_priority);

	//+++++++++++++++++++++++

	for (int counter = 0; counter < 4; counter++){
		//sleep(1);
	}

	return 0;
}

double makeRun( int wait, struct timespec start_time, struct timespec end_time){
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}
	waste_msecs(wait);
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &end_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}
	double msec_diff = calculateDiff( start_time, end_time);
	return msec_diff;
}

void waste_msecs(unsigned int msecs){
	int delay = (int) (100000 * msecs * corr_factor);
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
