#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* function1( void*  arg ){
   sleep(5);
   int *iptr = (int *) malloc(sizeof(int)); //allocate memory
   *iptr = pthread_self();
   printf( "This is function 1 with thread %d\n", pthread_self() );
   return iptr;
}

void* function2( void*  arg ){
   sleep(2);
   int *iptr = (int *) malloc(sizeof(int)); //allocate memory
   *iptr = pthread_self();
   printf( "This is function 2 with thread %d\n", pthread_self() );
   return iptr;
}

int main( void ){
   
   pthread_t thread_id;
   pthread_t thread_id_2;
   pthread_create(&thread_id, NULL, function1, NULL);
   pthread_create(&thread_id_2, NULL, function2, NULL);
   
   int *valueOfFunction1;
   int *valueOfFunction2;
   pthread_join(thread_id, &valueOfFunction1);
   pthread_join(thread_id_2, &valueOfFunction2);
   
   
   printf( "Value of function 1: %d\n", *valueOfFunction1);
   printf( "Value of function 2: %d\n", *valueOfFunction2);
   
   printf("Things should be over now");
   return 0;
}