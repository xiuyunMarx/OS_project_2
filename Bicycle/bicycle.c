#include "bicycle.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pthread.h"
#include "stdbool.h"
box_t box = {0, 0};
pthread_mutex_t boxMutex;
sem_t box_slots;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int totFrames, totWheels; 
void *frame_producer(void *arg) {
    int id = *(int *)arg; //this indicate the id for current frame producer
    while (1) {
        if(totFrames < M){
            sem_wait(&box_slots); // wait for an empty slot, and don't release here            
            pthread_mutex_lock(&boxMutex);
            totFrames++;            
            produce_frame(id);

            place_frame(id, &box);    //place frame into box, as well as increasing the number of frames in box
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&boxMutex);
            
        }else{
            break;
        }
    }
    pthread_exit(0);
}

void *wheel_producer(void *arg) {
    int id = *(int *)arg;
    while (1) {
        // some code goes here
        if(totWheels < 2*M){

            sem_wait(&box_slots); // wait for an empty slot, and don't release here
            pthread_mutex_lock(&boxMutex);
            produce_wheel(id);
            totWheels++;
        // produce only if the total wheels is less than 2M


            place_wheel(id, &box);            
            pthread_cond_signal(&cond);
            // int val;
            pthread_mutex_unlock(&boxMutex);
        }else{
            break;
        }
    }
    pthread_exit(0); 
}

void *assembler(void *arg) {
    int id = *(int *)arg;
    int totAssembled = 0;
    while (1) {
        if(totAssembled == M){
            break;
        }
        //wait until there is at least 1 frame
        pthread_mutex_lock(&boxMutex);
        while(box.n_frames < 1){
            pthread_cond_wait(&cond, &boxMutex);   
        }
        get_frame(id, &box);
        sem_post(&box_slots); // release one slot for frame
        pthread_mutex_unlock(&boxMutex);

        //wait until there are at least 2 wheels
        pthread_mutex_lock(&boxMutex);
        while(box.n_wheels < 2){
            pthread_cond_wait(&cond, &boxMutex);
        }
        get_wheels(id, &box);
        for(int i=0;i<2;i++)
            sem_post(&box_slots); // release two slots for wheels
        pthread_mutex_unlock(&boxMutex);

        assemble(id);
        totAssembled++;
    }
    pthread_exit(0);
}

void init() {
    pthread_mutex_init(&boxMutex, NULL);
    sem_init(&box_slots, false, N);
    totFrames =totWheels =0;
    // some code goes here
}

void destroy() {
    sem_destroy(&box_slots);
    pthread_mutex_destroy(&boxMutex);
    pthread_cond_destroy(&cond);
    // some code goes here
}