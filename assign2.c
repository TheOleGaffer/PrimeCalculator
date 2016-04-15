//  A multi-threaded program which prints out a given number of prime numbers based on user input
//
// Created by Sam on 2/25/2016.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

static int count = 2;         //Holds the value which threads check if prime
static int foundCount = 0;    //Keeps track of the number of found prime numbers
static int *arrayptr;         //An array which holds the prime numbers
static pthread_t *arrayThr;   //Array which holds the threads
pthread_mutex_t countLock;
pthread_mutex_t arrayLock;


//Checks if a number is prime
//Returns 1 if it is prime
//Returns 0 if not
int checkCandidate(int candidate){
    int bool = 1;
    int i;
    for(i = 2; i <= candidate/2; i++){
        if(candidate % i == 0 && i != candidate){
            bool = 0;
            return bool;
        }
    }
    return bool;
}

//Terminates all the created threads in an array
void terminateThreads(int num){
    int i;
    int sig = SIGTERM;
    for(i = 0; i < num; i++){
        pthread_kill(arrayThr[i] - 1, sig);
    }
}

//Finds a given number of prime numbers and adds them to an array
void *calcPrimes(void *n){
    int *numPrimes = (int *) n;
    while (foundCount < *numPrimes){
        pthread_mutex_lock(&countLock);         //mutex down
        int localCount = count;
        count++;
        pthread_mutex_unlock(&countLock);         //mutex up
        if(checkCandidate(localCount)){
            pthread_mutex_lock(&arrayLock);         //mutex down
            arrayptr[foundCount] = localCount;
            foundCount++;
            pthread_mutex_unlock(&arrayLock);        //mutex up
        }
    }
    return NULL;
}

//Checks if a number is above zero
int aboveZero(int num){
    int bool = 1;
    if(num < 1){
        bool = 0;
    }
    return bool;
}

//Prints out array of primes
void printArray(int length){
    int i;
    for(i = 0; i < length; i++){
        printf("Prime number %d: %d \n", (i + 1), *(arrayptr + i));
    }
}

int main(int argc, char* argv[]) {
    int numPrimes = 0;
    int numThreads = 3;  //default value for number of threads
    pthread_mutex_init(&countLock, NULL);
    pthread_mutex_init(&arrayLock, NULL);
    int i;

    //Takes input from the command line and validates it
    if (argc < 2) {
        fprintf(stderr, "%s: Too few arguments. \n", argv[0]);
        return 1;
    } else if (argc == 2){
        if(aboveZero(atoi(argv[1]))){
            numPrimes =  atoi(argv[1]);
        } else {
            fprintf(stderr, "%s: Invalid argument, needs to be above zero. \n", argv[0]);
            return 1;
        }
    } else if (argc == 3){
        if(aboveZero(atoi(argv[1]))){
            numPrimes =  atoi(argv[1]);
        } else {
            fprintf(stderr, "%s: Invalid argument, needs to be above zero.  \n", argv[0]);
            return 1;
        }
        if(aboveZero(atoi(argv[2]))){
            numThreads =  atoi(argv[2]);
        } else {
            fprintf(stderr, "%s: Invalid argument, needs to be above zero. \n", argv[0]);
            return 1;
        }
    } else if (argc > 3){
        fprintf(stderr, "%s: Too many arguments. \n", argv [0]);
        return 1;
    }

    arrayptr = (int *)malloc(numPrimes *sizeof(int));
    arrayThr = (pthread_t *)malloc(numThreads *sizeof(pthread_t));

    for (i = 0; i < numThreads; i++){
        pthread_t p_thread;
        pthread_create(&p_thread, NULL, calcPrimes, &numPrimes);
        arrayThr[i] = p_thread;
    }
    pthread_join(arrayThr[0],NULL);  //Waits for thread to find primes before terminating all of them

    terminateThreads(numThreads);    //Stops all the threads
    printArray(numPrimes);           //Prints out array of primes

    //Cleanup
    pthread_mutex_destroy(&countLock);
    pthread_mutex_destroy(&arrayLock);
    free(arrayptr);
    free(arrayThr);


    return 0;
}
