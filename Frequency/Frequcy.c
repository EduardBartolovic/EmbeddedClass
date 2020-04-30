/*
 ============================================================================
 Name        : hello.c
 Author      : Educhard
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(void) {
	struct timespec abs_start_time;
	struct timespec end_time;
	struct timespec start_time;
	int long erwartung;
	int long max =0;
	int long min = 1999999999;
	struct timespec validate_time;
	long int validate_nsec[10000];
	long int validate_sec[10000];

	if (-1 == clock_gettime(CLOCK_REALTIME, &start_time)){
		perror ("Wir haben die Uhr falsch gelesen");
		exit (EXIT_FAILURE);
	}
	abs_start_time = start_time;

	for (int i = 0; i < 10000; ++i)
	{
		if (start_time.tv_nsec >= 999000000 ){
			start_time.tv_sec += 1;
			start_time.tv_nsec = start_time.tv_nsec %  999000000;
		} else {
			start_time.tv_nsec += 1000000;
		}

    	if (-1 == clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &start_time, NULL)){
    		perror ("Wir haben länger als 1 ms gebraucht");
    		exit (EXIT_FAILURE);
    	}

    	if (-1 == clock_gettime(CLOCK_REALTIME, &validate_time)){
    		perror ("Beim Überprüfen der Zeit ist ein Fehler aufgetreten");
    		exit (EXIT_FAILURE);
    	}
    	validate_nsec[i] = validate_time.tv_nsec;
    	validate_sec[i] = validate_time.tv_sec;
	}

	if (-1 == clock_gettime(CLOCK_REALTIME, &end_time)){
		perror ("Beim Überprüfen der Zeit am Ende ist ein Fehler aufgetreten");
		exit (EXIT_FAILURE);
	}

	printf("Ende in sec %ld und msec %ld \n \n", end_time.tv_sec - abs_start_time.tv_sec, end_time.tv_nsec - abs_start_time.tv_nsec);


	for (int i = 1; i < 10000; ++i){
		if(validate_sec[i] > validate_sec[i-1]){
		//	erwartung = (validate_nsec[i] + 9990000000) - (validate_nsec[i-1]);
			printf("An stelle i = %d", i);
			printf("   validate_nscec = %ld     validate_nsec-1 = %ld \n", validate_nsec[i],validate_nsec[i-1]);
		} else {
			erwartung = validate_nsec[i] - validate_nsec[i-1];
		}

		if(erwartung > max){
			max = erwartung;
		} else if (erwartung < min){
			printf("alt = %ld", min);
			min = erwartung;
			printf("##### An stelle i = %d", i);
			printf(" # Min wurde unterschritten  : validate_nscec = %ld     validate_nsec-1 = %ld ### neuer min = %ld \n", validate_nsec[i],validate_nsec[i-1], min);
		}
		// printf("Zeit %d war %ld \n", i, erwartung);
	}
	printf("Minimale Wartezeit : %ld \n", min);
	printf("Maximale Wartezeit : %ld \n", max);
}
