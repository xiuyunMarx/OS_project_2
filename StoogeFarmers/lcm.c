#include "lcm.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "stdatomic.h"


extern const int N;
// semaphores
sem_t shovel;
const int ARRAY_SIZE = N;
atomic_bool *diggedHoles,*seededHoles, *filledHoles;
sem_t unfilledHoles;


pthread_mutex_t holeMutex;

void random_wait(){
    int T = rand() % 1000000 + 500000;
    usleep(T);
}
void *larry() {
    int id = 0;
    while(1){
        if(id >= ARRAY_SIZE) {
            bool flag = false;
            for(int i=0;i<ARRAY_SIZE;i++){
                if(diggedHoles[i] == false){  //there is at least one hole not digged
                    flag = true;
                    break;
                }
            }
            if(!flag) //all holes are digged
                break;
            else
                id = 0;
        }
        if(diggedHoles[id] == false){

            sem_wait(&unfilledHoles); //make sure no greater than MAX unfilled holes
            sem_wait(&shovel);
            get_shovel(LARRY);

            dig(LARRY, id+1);

            pthread_mutex_lock(&holeMutex);
            atomic_store(&diggedHoles[id], true); //set this hole as digged
            pthread_mutex_unlock(&holeMutex);

            drop_shovel(LARRY);   
            sem_post(&shovel);
         

            random_wait();
        }

        id++;
    }

    pthread_exit(0);
}


void *moe() {

    int id = 0;

    while(1){
        if(id>=ARRAY_SIZE){
            bool flag = false; 
            for(int i=0;i<ARRAY_SIZE;i++){
                if(seededHoles[i] == false){  //there is at least one hole not seeded
                    flag = true;
                    break;
                }
            }
            if(flag == false){ //all holes are seeded
                break;
            }else
                id = 0;
        }
        if(diggedHoles[id] == true && seededHoles[id] == false){
            plant(MOE,id+1);

            pthread_mutex_lock(&holeMutex);
            atomic_store(&seededHoles[id], true); //set this hole as seeded
            pthread_mutex_unlock(&holeMutex);

            random_wait();
        }
        id++;
    }

    pthread_exit(0);
}

void *curly() {
    int id = 0;

    while (1) {
        if(id >= ARRAY_SIZE) {
            bool flag = false;
            for(int i=0;i<ARRAY_SIZE;i++){
                if(filledHoles[i] == false){  //there is at least one hole not filled
                    flag = true;
                    break;
                }
            }
            if(flag == false){ //all holes are not filled
                break;
            }else {
                id = 0;
            }
        }
        if(seededHoles[id] == true && filledHoles[id] == false){
            sem_wait(&shovel);
            get_shovel(CURLY);

            fill(CURLY, id+1);

            pthread_mutex_lock(&holeMutex);
            atomic_store(&filledHoles[id], true); //set this hole as filled
            pthread_mutex_unlock(&holeMutex);

            sem_post(&unfilledHoles);
            drop_shovel(CURLY);            
            sem_post(&shovel);


            random_wait();
        }
        id++;
    }

    pthread_exit(0);
}

void init() {
    // some code goes here
    srand(time(0)) ;
    sem_init(&shovel, 0, 1);
    sem_init(&unfilledHoles, false, MAX);
    diggedHoles = malloc(sizeof(atomic_bool) * ARRAY_SIZE);
    seededHoles = malloc(sizeof(atomic_bool) * ARRAY_SIZE);
    filledHoles = malloc(sizeof(atomic_bool) * ARRAY_SIZE);
    for(int i = 0; i < ARRAY_SIZE; i++){
        atomic_store(&diggedHoles[i], false);
        atomic_store(&seededHoles[i], false);
        atomic_store(&filledHoles[i], false);
    }
    pthread_mutex_init(&holeMutex, NULL);
}

void destroy() {
    // some code goes here
    sem_destroy(&shovel);
    sem_destroy(&unfilledHoles);
    free(diggedHoles);
    free(seededHoles);
    free(filledHoles);
}
