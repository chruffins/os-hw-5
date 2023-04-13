/*
Question 1 of OS Homework 5 by Chris Lee

This program implements the barbershop problem.
=============================================
Barber thread algorithm:
=============================================
while true:
    if not awake:
        wait until awakened
        awake = true
    
    lock chair access
    signal barber busy

    if chairs not empty:
        pop customer from chairs
        unlock chair access
        cut hair
    else:
        go to sleep
        unlock chairs
    signal barber not busy
=============================================
=============================================
Customer thread algorithm:
=============================================
lock chairs
if no one in chairs and barber not busy:
    add self to chairs
    wake up barber
else if chairs not full:
    sit in waiting chairs
else:
    leave shop because full
=============================================
*/

#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#include "queue.h"

sem_t barber_alarm;
pthread_mutex_t chair_lock;
pthread_mutex_t barber_busy;

pthread_t barber;
pthread_t *customers;

queue waiting_chairs;

int n_customers;
int n_chairs;
int barber_help_time;
int customer_freq;

int customers_served;

void *barber_act() {
    int awake = 0;

    while (1) {
        if (!awake) {
            sem_wait(&barber_alarm);
            awake = 1;
        }

        pthread_mutex_lock(&chair_lock);
        pthread_mutex_lock(&barber_busy);
        if (!queue_empty(&waiting_chairs)) {
            int student_id = queue_pop(&waiting_chairs);
            printf("The barber is serving Customer %d now.\n", student_id);
            pthread_mutex_unlock(&chair_lock);
            sleep(rand() % 3 + 1);
        } else {
            if (customers_served >= n_customers - 1) {
                break;
            }
            printf("The barber is going to sleep now.\n");
            pthread_mutex_unlock(&chair_lock);
            awake = 0;
        }
        pthread_mutex_unlock(&barber_busy);
    }

    printf("The barber is finished cutting customers' hair.\n");
}

void *customers_act(void *id) {
    int own_id = (unsigned long long) id;

    pthread_mutex_lock(&chair_lock);
    if (queue_empty(&waiting_chairs) && pthread_mutex_trylock(&barber_busy) == 0) {
        queue_add(&waiting_chairs, own_id);
        printf("Customer %d woke up the barber.\n", own_id);
        sem_post(&barber_alarm);
        pthread_mutex_unlock(&barber_busy);
        pthread_mutex_unlock(&chair_lock);
    } else if (!queue_full(&waiting_chairs)) {
        queue_add(&waiting_chairs, own_id);
        printf("Customer %d sat down in a waiting chair.\n", own_id);
        pthread_mutex_unlock(&chair_lock);
    } else {
        printf("Customer %d exploded and died.\n", own_id);
        pthread_mutex_unlock(&chair_lock);
    }
}

int main(int argc, char **argv) {
    n_customers = 10;
    n_chairs = 3;
    //customer_freq = 1;

    switch (argc)
    {
    case 5:
        srand(atoi(argv[4]));
    case 4:
        customer_freq = atoi(argv[3]);
    case 3:
        n_customers = atoi(argv[2]);
    case 2:
        n_chairs = atoi(argv[1]);
        break;
    default:
        printf("usage: part1 [chairs] [customers] [customer frequency] [random seed]\n");
        printf("defaults: 3 10 random\n");
    }

    waiting_chairs = queue_create(n_chairs);

    pthread_mutex_init(&chair_lock, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&barber_busy, PTHREAD_MUTEX_NORMAL);
    sem_init(&barber_alarm, 0, 0);
    pthread_create(&barber, NULL, barber_act, NULL);

    customers = malloc(n_customers * sizeof(pthread_t));
    
    printf("There are %d chairs and %d customers.\nStarting simulation.\n", n_chairs, n_customers);

    for (customers_served = 0; customers_served < n_customers; customers_served++) {
        pthread_create(&customers[customers_served], NULL, customers_act, (void*)(long long)customers_served);
        sleep(sleep(rand() % 3));
    }

    pthread_join(barber, NULL);

    printf("The barber shop has closed.\n");

    return 0;
}