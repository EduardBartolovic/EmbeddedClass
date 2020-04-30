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
	struct timespec abs_start_time;	 // Anfangszeit
	struct timespec end_time;	// Endzeit
	struct timespec start_time;	//1Ms Takt, wird bei jeder schleife um eine MS erhöht
	int long erwartung;
	int long max = 0; // größte Wartezeit zwischen zwei Takten
	int long min = 1999999999;	//geringste Wartezeit zwischen zwei Takten
	int long LOOP_ITERATIONS = 10000;
	int long MIO = 1000000;
	int long WRAP_AROUNT = 999000000;	// 1Mrd wird zu 0 => Wraparound detektiert 1Mrd - 1Mio, eine Iteration vor Wraparound
	struct timespec validate_time;
	long int validate_nsec[LOOP_ITERATIONS]; //validiere die verstrichenen NS
	long int validate_sec[LOOP_ITERATIONS]; //validiere die verstrichenen SEc

	if (-1 == clock_gettime(CLOCK_REALTIME, &start_time)){
		perror ("Wir haben die Uhr falsch gelesen");
		exit (EXIT_FAILURE);
	}
	abs_start_time = start_time;

	for (int i = 0; i < LOOP_ITERATIONS; ++i)
	{
		if (start_time.tv_nsec >= WRAP_AROUNT ){
			start_time.tv_sec += 1;
			start_time.tv_nsec = start_time.tv_nsec %  WRAP_AROUNT;
		} else {
			start_time.tv_nsec += MIO;
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


	for (int i = 1; i < LOOP_ITERATIONS; ++i){
		if(validate_sec[i] > validate_sec[i-1]){
			erwartung = (validate_nsec[i] + WRAP_AROUNT + MIO) - (validate_nsec[i-1]);
			printf("An stelle i = %d", i);
			printf("   validate_nscec = %ld     validate_nsec-1 = %ld \n", validate_nsec[i],validate_nsec[i-1]);
		} else {
			erwartung = validate_nsec[i] - validate_nsec[i-1];
		}

		if(erwartung > max){
			printf("alt = %ld", max);
			max = erwartung;
			printf("****** An stelle i = %d", i);
			printf(" # max wurde überschritten  : validate_nscec = %ld     validate_nsec-1 = %ld ### neuer max  = %ld \n", validate_nsec[i],validate_nsec[i-1], max);

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
