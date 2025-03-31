#include <stdio.h>
#include <pthread.h>
#include "stdlib.h"
int variable = 0;
int threshold = 10;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* increment_thread(void* arg) {
    for (int i = 0; i < 20; i++) {
        pthread_mutex_lock(&mutex);
        variable++;
        printf("Incremented: %d\n", variable);
        pthread_cond_signal(&cond); // Notify waiting threads
        pthread_mutex_unlock(&mutex);
        sleep(1); // Simulating some work
    }
    return NULL;
}

void* wait_thread(void* arg) {
    pthread_mutex_lock(&mutex);
    while (variable <= threshold) {
        printf("Waiting... Current value: %d\n", variable);
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Variable exceeded threshold! Final value: %d\n", variable);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment_thread, NULL);
    pthread_create(&t2, NULL, wait_thread, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
