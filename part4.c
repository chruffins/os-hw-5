/*
Question 4 of OS Homework 5 by Chris Lee

This program implements the readers-writers problem.
=============================================
Writer thread algorithm:
=============================================
signal to prevent new readers from reading
wait until database open
lock database
do work
unlock database
=============================================
=============================================
Reader thread algorithm:
=============================================
wait if writer is waiting
increment current readers
if only reader then lock database from writers
do work
if last reader then unlock database for writers
=============================================
*/

#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>

pthread_mutex_t access_mutex;
pthread_mutex_t writer_mutex;
pthread_mutex_t c_readers_mutex;

int completed;
int current_readers;

int n_accessors;

void *writer_work(void *vres) {
    int res = (int)(unsigned long long) vres;

    printf("Writer %d lined up!\n", res);
    pthread_mutex_lock(&writer_mutex);
    printf("Writer %d blocking noobs from reading now.\n", res);
    pthread_mutex_lock(&access_mutex);
    printf("Writer %d is writing!\n", res);
    sleep(2);
    printf("Writer %d finished!\n", res);
    completed++;
    pthread_mutex_unlock(&access_mutex);
    pthread_mutex_unlock(&writer_mutex);
}

void *reader_work(void *vres) {
    int res = (int)(unsigned long long) vres;

    printf("Reader %d lined up!\n", res);
    pthread_mutex_lock(&writer_mutex); // force wait until writer is done
    pthread_mutex_unlock(&writer_mutex); // we didn't actually need the mutex

    pthread_mutex_lock(&c_readers_mutex);
    current_readers++;
    if (current_readers == 1) {
        pthread_mutex_lock(&access_mutex);
    }
    pthread_mutex_unlock(&c_readers_mutex);

    printf("Reader %d is reading!\n", res);
    sleep(3);
    printf("Reader %d finished!\n", res);

    pthread_mutex_lock(&c_readers_mutex);
    completed++;
    current_readers--;
    if (current_readers == 0) {
        printf("Last reader %d has turned off the lights.\n", res);
        pthread_mutex_unlock(&access_mutex);
    }
    pthread_mutex_unlock(&c_readers_mutex);
}

int main(int argc, char **argv) {
    char access_queue[10];
    pthread_t threads[10];

    n_accessors = 10;

    pthread_mutex_init(&access_mutex, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&writer_mutex, PTHREAD_MUTEX_NORMAL);

    if (argc == 2) {
        strncpy(access_queue, argv[1], 10);
    } else {
        printf("usage: part4 [10 char string containing r and w only]\n");

        for (int i = 0; i < 10; i++) {
            int new_guy = rand() % 2;
            switch (new_guy)
            {
            case 0:
                access_queue[i] = 'r';
                break;
            case 1:
                access_queue[i] = 'w';
                break;
            default:
                break;
            }
        }
    }

    n_accessors = 0;

    for (int i = 0; i < 10; i++) {
        switch (access_queue[i])
        {
        case 'r':
            pthread_create(&threads[i], NULL, reader_work, (void*)(unsigned long long)i);
            n_accessors++;
            break;
        case 'w':
            pthread_create(&threads[i], NULL, writer_work, (void*)(unsigned long long)i);
            n_accessors++;
            break;
        default:
            break;
        }
        sleep(1);
    }

    while (completed < n_accessors) {
        sleep(1);
    }

    return 0;
}