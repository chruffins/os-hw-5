/*
Question 3 of OS Homework 5 by Chris Lee

This program implements the TA office hours problem.
=============================================
TA thread algorithm:
=============================================
while true:
    if not awake:
        wait until awakened
        awake = true
    
    lock chair access
    signal TA busy

    if chairs not empty:
        pop student from chairs
        unlock chair access
        help student
    else:
        go to sleep
        unlock chairs
    signal TA not busy
=============================================
=============================================
Student thread algorithm:
=============================================
lock chairs
if no one in chairs and TA not busy:
    add self to chairs
    wake up TA
else if chairs not full:
    sit in waiting chairs
else:
    sleep for a few seconds
    go back to step 1
=============================================
*/

#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>

#include "queue.h"

#define NUM_queue 3

int n_students;
int ta_help_time;
int student_freq;
int id;

sem_t student_alarm;
pthread_mutex_t chair_lock;
pthread_mutex_t ta_busy;

queue waiting_chairs;

pthread_t *student_threads;
pthread_t ta_thread;

void *ta_work() {
    int awake = 0;
    int students_helped = 0;

    while (students_helped < n_students) {
        if (awake == 0) {
            sem_wait(&student_alarm);
            awake = 1;
        }
        printf("The TA is ready for another student.\n");
        pthread_mutex_lock(&ta_busy);
        pthread_mutex_lock(&chair_lock);
        if (!queue_empty(&waiting_chairs)) {
            int student_id = queue_pop(&waiting_chairs);
            printf("The TA is helping Student %d now.\n", student_id);
            pthread_mutex_unlock(&chair_lock);
            sleep(ta_help_time);
            students_helped++;
        } else {
            printf("The TA is going to sleep now.\n");
            awake = 0;
            pthread_mutex_unlock(&chair_lock);
        }
        pthread_mutex_unlock(&ta_busy);
    }

    printf("The TA has survived office hours!\n");
}

void *student_work() {
    int sitting = 0;
    int own_id = id;
    id++;

    while(!sitting) {
        pthread_mutex_lock(&chair_lock);
        if (queue_empty(&waiting_chairs) && pthread_mutex_trylock(&ta_busy) == 0) {
            queue_add(&waiting_chairs, own_id);
            printf("Student %d woke up the TA.\n", own_id);
            sem_post(&student_alarm);
            pthread_mutex_unlock(&ta_busy);
            pthread_mutex_unlock(&chair_lock);
            sitting = 1;
        } else if (!queue_full(&waiting_chairs)) {
            queue_add(&waiting_chairs, own_id);
            printf("Student %d sat down in a waiting chair.\n", own_id);
            pthread_mutex_unlock(&chair_lock);
            sitting = 1;
        } else {
            printf("Student %d will come back in 5 seconds.\n", own_id);
            pthread_mutex_unlock(&chair_lock);
            sleep(5);
        }
    }
}

int main(int argc, char **argv) {
    sem_init(&student_alarm, 0, 0);
    pthread_mutex_init(&chair_lock, PTHREAD_MUTEX_NORMAL);

    n_students = 5;
    ta_help_time = 2;
    student_freq = 1;

    switch (argc)
    {
    case 4:
        student_freq = atoi(argv[3]);
    case 3:
        ta_help_time = atoi(argv[2]);
    case 2:
        n_students = atoi(argv[1]);
        break;
    }

    waiting_chairs = queue_create(NUM_queue);

    student_threads = malloc(n_students * sizeof(pthread_t));

    printf("Inputs: %d students, %d seconds for TA to help, %d seconds for students to appear\n", n_students, ta_help_time, student_freq);

    pthread_create(&ta_thread, NULL, ta_work, NULL);

    for (int i = 0; i < n_students; i++) {
        pthread_create(&student_threads[i], NULL, student_work, NULL);
        sleep(student_freq);
    }

    pthread_join(ta_thread, NULL);

    return 0;
}