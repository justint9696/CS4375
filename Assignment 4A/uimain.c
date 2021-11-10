#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_INPUT_LEN 1024
#define MAX_NUM_THREADS 1024

volatile static int counter = 0;

pthread_mutex_t lock;

void *counter_thread(void *arg) {
	// updates counter by 1 n number of times
	int n = *(int *)arg;
	for (int i = 0; i < n; i++) {
		counter++;
	}
	return NULL;
}

void *counter_thread_lock(void *arg) {
	// thread is locked each time it is joined, if lock is taken, thread spins until it is free
	pthread_mutex_lock(&lock);
	int n = *(int *)arg;
	for (int i = 0; i < n; i++) {
		counter++;
	}
	pthread_mutex_unlock(&lock);
	// thread releases lock for next thread to take
	return NULL;
}

void *counter_increment(void *arg) {
	int n = *(int *)arg;
	for (int i = 0; i < n; i++) {
		printf("%i\n", i);
	}
	return NULL;
}

void *counter_increment_lock(void *arg) {
	pthread_mutex_lock(&lock);
	int n = *(int *)arg;
	for (int i = 0; i < n; i++) {
		printf("%i\n", i);
	}
	pthread_mutex_unlock(&lock);
	return NULL;
}

int main() {
	int i;
	int n, t;
	int syncronization;
	int start, end;
	char input[MAX_INPUT_LEN];
	pthread_t th[MAX_NUM_THREADS];
	
	while (1) {
		// take input for number of iterations
		printf("Enter a value for N\n$ ");
		gets(input);
		n = atoi(input);
		
		// take input for number of threads
		printf("Enter a value for T\n$ ");
		gets(input);
		t = atoi(input);
		
		// check if t is within max thread count
		if (t < MAX_NUM_THREADS) {
			printf("Number of iterations: %i\nNumber of threads: %i\n\n", n, t);
			
			start = counter;
			for (i = 0; i < t; i++)
				pthread_create(&th[i], NULL, &counter_thread, &n);
			for (i = 0; i < t; i++)
				pthread_join(th[i], NULL);
			end = counter;
			printf("Target counter value: %i\nResult without lock: %i\n", n*t, end-start);
			
			start = counter;
			for (i = 0; i < t; i++)
				pthread_create(&th[i], NULL, &counter_thread_lock, &n);
			for (i = 0; i < t; i++)
				pthread_join(th[i], NULL);
			end = counter;
			
			printf("Result with lock: %i\n\n", end-start);
			
			while (1) {
				// prompts user to test syncronization
				printf("Test syncronization? \n1: Yes \n2: No\n$ ");
				gets(input);
				syncronization = atoi(input);
				if (syncronization == 1) {
					// takes input for number of iterations
					printf("Enter a value for N\n$ ");
					gets(input);
					n = atoi(input);
					
					// takes input for number of threads
					printf("Enter a value for T\n$ ");
					gets(input);
					t = atoi(input);
					
					if (t < MAX_NUM_THREADS) {
						printf("Counter increment without lock: \n");
						for (i = 0; i < t; i++)
							pthread_create(&th[i], NULL, &counter_increment, &n);
						for (i = 0; i < t; i++)
							pthread_join(th[i], NULL);
						
						printf("\nCounter increment with lock: \n");
						for (i = 0; i < t; i++)
							pthread_create(&th[i], NULL, &counter_increment_lock, &n);
						for (i = 0; i < t; i++)
							pthread_join(th[i], NULL);
					} else {
						printf("T-value of [%i] exceeds maximum number of thread: %i\n", t, MAX_NUM_THREADS);
					}
				} else if (syncronization == 2) {
					// breaks to main loop
					break;
				} else {
					printf("Invalid input: %i\n", syncronization);
				}
			}
		} else {
			// added a capacity limit on threads because with a large number of threads, the result would never return
			printf("T-value of [%i] exceeds maximum number of thread: %i\n", t, MAX_NUM_THREADS);
		}
	}
}	
