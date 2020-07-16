/*
 ============================================================================
 Name        : Aufgabe4.c
 Author      : Eduard Bartolovic, Richard Reik
 Version     :
 Copyright   : This is ours, dont touch this
 Description : 2 Verschiedene Threads arbeiten zusammen
 ============================================================================
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
#include <semaphore.h>

double corr_factor = 1.0;
sem_t mutex;

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
	pthread_t thread_id_zwei;
	pthread_attr_t attr;


	if (-1 == sem_init(&mutex, 0 , 0)){
		perror ("Error in sem_init");
		exit (EXIT_FAILURE);
	}

	if (-1 == pthread_attr_init(&attr)){
		perror ("Error in pthread_attr_init");
		exit (EXIT_FAILURE);
	}

	if (-1 == pthread_create(&thread_id, &attr, threadFunction, NULL)){
		perror ("Error in Create pThread1");
		exit (EXIT_FAILURE);
	}

	 if (-1 == pthread_create(&thread_id_zwei, &attr, threadFunctionZwei, NULL)){
		perror ("Error in Create pThread2");
		exit (EXIT_FAILURE);
	}

	if (-1 == pthread_join(thread_id, NULL)){
		perror ("Error in JOIN pThread1");
		exit (EXIT_FAILURE);
	}

	if (-1 == pthread_join(thread_id_zwei, NULL)){
		perror ("Error in JOIN pThread2");
		exit (EXIT_FAILURE);
	}

	sem_destroy(&mutex);

	return EXIT_SUCCESS;
}

/**
 * Kalibrierung der WasteMsec
 */
void prepEverything(unsigned int wait){
	//Estimate optimal corr_factor
    struct timespec start_time;
	struct timespec end_time;
	int LOOPITERATIONS = 10;

	wait = 3;
	corr_factor = 1.0;

	makeRun( wait, start_time, end_time );//warm up (waste the first call)

	//+++++++++++++++++++++++

	double rekord = 10000000;
	double msec_diff_run;
	for(int counter = 0 ; counter < LOOPITERATIONS; counter++){
		msec_diff_run = makeRun( wait, start_time, end_time );
		if (msec_diff_run < rekord){
			rekord = msec_diff_run;
		}
	}
	corr_factor = wait / rekord;
	printf("\nCalibration Ready => Corr: %lf \n", corr_factor);
	return;
}

/**
 * Thread 1.
 */
void* threadFunction( void* arg){
	int long LOOP_ITERATIONS = 49;
	struct timespec start_time;
	struct timespec validate_time;
	int long WRAP_AROUNT = 996000000;
	int long MIO = 4000000;
	long int validate_nsec[LOOP_ITERATIONS];


	//Setze die Priorität und den Scheduling algorithmus + Ausgabe auf der Console
	struct sched_param sh_param;
	int policy = SCHED_FIFO;
	sh_param.sched_priority = 49;
	if (-1 == sched_setscheduler(0,policy, &sh_param)){
		perror ("Error in set schedular in Thread 1");
		exit (EXIT_FAILURE);
	}
	if (-1 == pthread_getschedparam(pthread_self(), &policy, &sh_param)){
		perror ("Error while retreving schedparam in Thread 1");
		exit (EXIT_FAILURE);
	}
	printf("Priority=%d\n", sh_param.__sched_priority);


	//Eigentliche Funktionalität
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time (start_time) in Thread 1");
		exit (EXIT_FAILURE);
	}
	for (int i = 0; i < LOOP_ITERATIONS; ++i) {
			if (start_time.tv_nsec >= WRAP_AROUNT ){ //Falls ein Sekunden wraparound passieren sollte.
				start_time.tv_sec += 1;
				start_time.tv_nsec = start_time.tv_nsec %  WRAP_AROUNT;
			} else {
				start_time.tv_nsec += MIO; //sonst 4ms warten
			}
			//if(i % 3 == 0){ // hol dir den Mutex
			//	if (-1 == sem_wait(&mutex)){
			//		perror ("Error in sem_wai in pThread1");
			//		exit (EXIT_FAILURE);
			//	}
			//}
			waste_msecs(2); // verbrauche 2 MS
	    	if (-1 == clock_gettime(CLOCK_MONOTONIC, &validate_time)){ //Holen der Aktuellen Zeit für die Validierung
	    		perror ("Error in get Time (validate_time) in Thread 1");
	    		exit (EXIT_FAILURE);
	    	}
			if(i%3 == 2){  // gib den mutex wieder frei
				if (-1 == sem_post(&mutex)){
					perror ("Error in sem_post in Thread 1");
					exit (EXIT_FAILURE);
				}
			}
	    	if (-1 == clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &start_time, NULL)){ //warten bis Zielzeit erreicht
	    		perror ("Error in Sleep in Thread 1");
	    		exit (EXIT_FAILURE);
	    	}
	    	validate_nsec[i] = start_time.tv_nsec - validate_time.tv_nsec; //Speichern der Validierungszeiten
		}
	for (int i = 0; i < LOOP_ITERATIONS; ++i){
			printf("NSekunde = %ld \n\n",validate_nsec[i]);
		}

	return 0;
}

/**
 * Thread 2
 */
void* threadFunctionZwei( void* arg){
	unsigned int LOOP_ITERATIONS = 16;
	struct timespec start_time;
	struct timespec end_time;
	long int validate_nsec[LOOP_ITERATIONS]; //validiere die verstrichene Zeit


	//Setze die Priorität und den Scheduling algorithmus + Ausgabe auf der Console
	struct sched_param sh_param;
	int policy = SCHED_FIFO;
	sh_param.sched_priority = 48;
	if (-1 == sched_setscheduler(0,policy, &sh_param)){
		perror ("Error in set schedular in Thread 2");
		exit (EXIT_FAILURE);
	}
	if (-1 == pthread_getschedparam(pthread_self(), &policy, &sh_param)){
		perror ("Error while retreving schedparam in Thread 2");
		exit (EXIT_FAILURE);
	}
	printf("Priority=%d\n", sh_param.__sched_priority);


	//Eigentliche Funktionalität
	for (int counter = 0; counter < LOOP_ITERATIONS; counter++){
		if (-1 == sem_wait(&mutex)){
			perror ("Error in sem_wait in Thread 2");
			exit (EXIT_FAILURE);
		}
		//if (-1 == sem_post(&mutex)){
		//	perror ("Error in sem_post in Thread 2");
		//	exit (EXIT_FAILURE);
		//}
		if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
			perror ("Error in get Time in Thread 2");
			exit (EXIT_FAILURE);
		}
		waste_msecs(3); // verbrauche 3 MS
		if (-1 == clock_gettime(CLOCK_MONOTONIC, &end_time)){ //Holen der Aktuellen Zeit
			perror ("Error in get Time in Thread 2");
			exit (EXIT_FAILURE);
		}
    	validate_nsec[counter] = end_time.tv_nsec - start_time.tv_nsec; //Speichern der Validierungszeiten
	}
	sleep(2); // damit die Prints sich nicht in die Quere kommen von p1 und p2
	for (int i = 0; i < LOOP_ITERATIONS; ++i) {
			printf("Thread 2 : NSekunde = %ld \n\n", validate_nsec[i]);
		}
	return 0;
}

/**
 * WasteMsec mit Zeiterfassung
 */
double makeRun( int wait, struct timespec start_time, struct timespec end_time){
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time in makeRun");
		exit (EXIT_FAILURE);
	}
	waste_msecs(wait);
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &end_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time in makeRun");
		exit (EXIT_FAILURE);
	}
	double msec_diff = calculateDiff( start_time, end_time);
	return msec_diff;
}

/**
 * Zeitdifferenz mit WrapAround berücksichtigt
 */
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

void waste_msecs(unsigned int msecs){
	int delay = (int) (100000 * msecs * corr_factor);
	volatile int dummy;
	for(int counter = 0; counter < delay ;counter++){
		dummy++;
	}
}
