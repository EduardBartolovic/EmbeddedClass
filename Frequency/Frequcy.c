/*
 ============================================================================
 Name        : hello.c
 Author      : Richard Reik, Eduard Bartolovic
 Description : Generates a Frequency of one Ms
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

	if (-1 == clock_gettime(CLOCK_REALTIME, &start_time)){ //Holen der Aktuellen Zeit
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}
	abs_start_time = start_time;  //Speichern der Absolutenstartzeit

	for (int i = 0; i < LOOP_ITERATIONS; ++i) //Periodenschleife
	{
		if (start_time.tv_nsec >= WRAP_AROUNT ){ //Falls ein Sekunden wraparound passieren sollte.
			start_time.tv_sec += 1;
			start_time.tv_nsec = start_time.tv_nsec %  WRAP_AROUNT;
		} else {
			start_time.tv_nsec += MIO; //sonst 1ms warten
		}

    	if (-1 == clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &start_time, NULL)){ //warten bis Zielzeit erreicht
    		perror ("Error in WAIT");
    		exit (EXIT_FAILURE);
    	}

    	if (-1 == clock_gettime(CLOCK_REALTIME, &validate_time)){ //Holen der Aktuellen Zeit für die Validierung
    		perror ("Error in get Time");
    		exit (EXIT_FAILURE);
    	}
    	validate_nsec[i] = validate_time.tv_nsec; //Speichern der Validierungszeiten sec
    	validate_sec[i] = validate_time.tv_sec;//Speichern der Validierungszeiten nano sec
	}

	if (-1 == clock_gettime(CLOCK_REALTIME, &end_time)){
		perror ("Error in get Time");
		exit (EXIT_FAILURE);
	}

	//Gesamte Programzeit
	printf("Ende in sec %ld und msec %ld \n \n", end_time.tv_sec - abs_start_time.tv_sec, end_time.tv_nsec - abs_start_time.tv_nsec);

	for (int i = 1; i < LOOP_ITERATIONS; ++i){	//Validierung
		if(validate_sec[i] > validate_sec[i-1]){ // Für Wraparound
			erwartung = (validate_nsec[i] + WRAP_AROUNT + MIO) - (validate_nsec[i-1]); //Offset um eine Sekunde
			//printf("An stelle i = %d", i);
			//printf("   validate_nscec = %ld     validate_nsec-1 = %ld \n", validate_nsec[i],validate_nsec[i-1]);
		} else {
			erwartung = validate_nsec[i] - validate_nsec[i-1];
		}

		if(erwartung > max){ //neuer Rekord in maximaler Wartezeit
			max = erwartung;
			//printf("****** An stelle i = %d", i);
			//printf(" # max wurde überschritten  : validate_nscec = %ld     validate_nsec-1 = %ld ### neuer max  = %ld \n", validate_nsec[i],validate_nsec[i-1], max);
		} else if (erwartung < min){ //neuer Rekord in minimaler Wartezeit
			min = erwartung;
			//printf("##### An stelle i = %d", i);
			//printf(" # Min wurde unterschritten  : validate_nscec = %ld     validate_nsec-1 = %ld ### neuer min = %ld \n", validate_nsec[i],validate_nsec[i-1], min);
		}
		// printf("Zeit %d war %ld \n", i, erwartung);
	}
	printf("Minimale Wartezeit : %ld \n", min);
	printf("Maximale Wartezeit : %ld \n", max);
}
