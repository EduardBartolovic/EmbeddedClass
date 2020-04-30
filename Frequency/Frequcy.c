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
    long int validate[100];
    struct timespec validate_time;

    if (-1 == clock_gettime(CLOCK_REALTIME, &start_time)){
        perror ("Wir haben die Uhr falsch gelesen");
        exit (EXIT_FAILURE);
    }
    abs_start_time = start_time;

    for (int i = 0; i < 10000; ++i)
    {
        if (start_time.tv_nsec >= 999000000 ){
            start_time.tv_sec += 1;
        }
        start_time.tv_nsec += 1000000;
        if (-1 == clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &start_time, NULL)){
            perror ("Wir haben länger als 1 ms gebraucht");
            exit (EXIT_FAILURE);
        }

        if (-1 == clock_gettime(CLOCK_REALTIME, &validate_time)){
            perror ("Beim Überprüfen der Zeit ist ein Fehler aufgetreten");
            exit (EXIT_FAILURE);
        }
        validate[i] = validate_time.tv_nsec;
    }

    if (-1 == clock_gettime(CLOCK_REALTIME, &end_time)){
        perror ("Beim Überprüfen der Zeit am Ende ist ein Fehler aufgetreten");
        exit (EXIT_FAILURE);
    }

    printf("Ende in sec %ld und msec %ld \n \n", end_time.tv_sec - abs_start_time.tv_sec, end_time.tv_nsec - abs_start_time.tv_nsec);
    int long erwartung;
    for (int i = 1; i < 10000; ++i){
        erwartung = validate[i] - validate[i-1];
        printf("Zeit %d war %ld \n", i, erwartung);
    }
}
