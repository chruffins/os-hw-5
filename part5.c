/*
Question 5 of OS Homework 5 by Chris Lee

This program implements the starvation-free
version of the bridge problem.

=============================================
Farmer thread algorithm (NOT starvation-free):
=============================================
Lock bridge mutex
Cross
Unlock bridge mutex
=============================================
=============================================
Farmer thread algorithm (starvation-free):
=============================================
Wait for farmer from other side to cross first if there exists a farmer
Lock bridge mutex
Cross
Unlock bridge mutex
Signal for farmer on your side or other side to cross bridge
=============================================
*/

#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>

enum DIRECTION {
    NORTHBOUND=0,
    SOUTHBOUND=1
};

sem_t northbound_sem;
sem_t southbound_sem;
pthread_mutex_t bridge_mutex;
pthread_mutex_t farmer_count;
sem_t farmers_done;

int northbound_farmers;
int southbound_farmers;

void *farmer_work(void *res) {
    unsigned long long direction = (unsigned long long)res;

    switch (direction)
    {
    case NORTHBOUND:
        sem_wait(&northbound_sem);
        break;
    case SOUTHBOUND:
        sem_wait(&southbound_sem);
        break;
    default:
        break;
    }

    pthread_mutex_lock(&bridge_mutex);
    if (direction == NORTHBOUND) {
        printf("A northbound farmer is crossing now!\n");
    } else if (direction == SOUTHBOUND) {
        printf("A southbound farmer is crossing now!\n");
    } 
    sleep((rand() % 2) + 1);
    pthread_mutex_lock(&farmer_count);
    if (direction == NORTHBOUND) {
        northbound_farmers--;
    } else if (direction == SOUTHBOUND) {
        southbound_farmers--;
    }
    printf("The farmer has crossed!\n");

    if (!northbound_farmers && !southbound_farmers) {
        sem_post(&farmers_done);
    }
    pthread_mutex_unlock(&bridge_mutex);

    switch (direction)
    {
    case NORTHBOUND:
        if (!southbound_farmers) {
            sem_post(&northbound_sem);
        } else {
            sem_post(&southbound_sem);
        }
        break;
    case SOUTHBOUND:
        if (!northbound_farmers) {
            sem_post(&southbound_sem);
        } else {
            sem_post(&northbound_sem);
        }
    default:
        break;
    }
    pthread_mutex_unlock(&farmer_count);
}

int main(int argc, char **argv) {
    sem_init(&northbound_sem, 0, 1);
    sem_init(&southbound_sem, 0, 0);
    sem_init(&farmers_done, 0, 0);
    pthread_mutex_init(&bridge_mutex, NULL);

    northbound_farmers = 0;
    southbound_farmers = 0;

    pthread_t *farmers;
    int n_farmers;

    if (argc == 2) {
        n_farmers = atoi(argv[1]);
    } else {
        n_farmers = 10;
    }

    srand(time(NULL));

    farmers = malloc(n_farmers * sizeof(pthread_t));

    int nb = 0;
    int sb = 0;

    printf("Simulation started.\n");

    pthread_create(&farmers[0], NULL, farmer_work, (void*)0);
    for (int i = 1; i < n_farmers; i++) {
        unsigned long long dir = rand() % 2;
        pthread_create(&farmers[i], NULL, farmer_work, (void*)dir);

        pthread_mutex_lock(&farmer_count);
        if (dir == NORTHBOUND) {
            nb++;
            northbound_farmers++;
        } else {
            sb++;
            southbound_farmers++;
        }
        pthread_mutex_unlock(&farmer_count);
    }

    printf("%d northbound farmers and %d southbound farmers were created.\n", nb, sb);

    sem_wait(&farmers_done);

    printf("Simulation ended.\n");
    free(farmers);
}