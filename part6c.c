/*
Question 6c of OS Homework 5 by Chris Lee

This program implements the throughput maximization
version of the student crossing problem.
*/

#include<unistd.h>
#include<semaphore.h>
#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

enum DESTINATION {
    WEST,
    EAST
} typedef DESTINATION;

struct os_student_t {
    int id;
    DESTINATION dest;
} typedef student;

void wait_until_safe_to_cross(DESTINATION d);
void cross_ravine(int id, DESTINATION d);
void done_with_crossing(DESTINATION d);

short max_east;
short max_west;
short max_total;

short eastbound;
short westbound;
short total;

sem_t safe_for_west;
sem_t safe_for_east;
pthread_mutex_t rope_mutex;

int completions;

void wait_until_safe_to_cross(DESTINATION d) {
    while (1) {
    switch (d)
    {
    case WEST:
        pthread_mutex_lock(&rope_mutex);
        if (westbound == max_west || total == max_total) {
            pthread_mutex_unlock(&rope_mutex);
            sem_wait(&safe_for_west);
        } else {
            westbound++;
            total++;
            pthread_mutex_unlock(&rope_mutex);
            return;
        }
        break;
    case EAST:
        pthread_mutex_lock(&rope_mutex);
        if (eastbound == max_east || total == max_total) {
            pthread_mutex_unlock(&rope_mutex);
            sem_wait(&safe_for_east);
        } else {
            eastbound++;
            total++;
            pthread_mutex_unlock(&rope_mutex);
            return;
        }
        break;
    }
    }
    return;
}

void cross_ravine(int id, DESTINATION d) {
    switch (d)
    {
    case WEST:
        printf("WEST %d STARTED (%d ON ROPE NOW)\n", id, total);
        break;
    case EAST:
        printf("EAST %d STARTED (%d ON ROPE NOW)\n", id, total);
        break;
    }

    sleep((rand() % 3) + 3);

    switch (d)
    {
    case WEST:
        printf("WEST %d FINISHED\n", id);
        break;
    case EAST:
        printf("EAST %d FINISHED\n", id);
        break;
    }
    return;
}

void done_with_crossing(DESTINATION d) {
    pthread_mutex_lock(&rope_mutex);
    switch (d)
    {
    case WEST:
        westbound--;
        break;
    case EAST:
        eastbound--;
        break;
    default:
        break;
    }

    total--;
    completions++;
    sem_post(&safe_for_east);
    sem_post(&safe_for_west);

    pthread_mutex_unlock(&rope_mutex);
}

void *student_work(void *vself) {
    student *self = (student*)vself;

    switch (self->dest)
    {
    case WEST:
        printf("WEST %d ARRIVED\n", self->id);
        break;
    case EAST:
        printf("EAST %d ARRIVED\n", self->id);
        break;
    default:
        break;
    }

    wait_until_safe_to_cross(self->dest);
    cross_ravine(self->id, self->dest);
    done_with_crossing(self->dest);
}

int main(int argc, char **argv) {
    max_west = 3;
    max_east = 3;
    max_total = 5;

    eastbound = 0;
    westbound = 0;
    total = 0;

    sem_init(&safe_for_west, 0, 0);
    sem_init(&safe_for_east, 0, 0);
    pthread_mutex_init(&rope_mutex, NULL);

    pthread_t *students;
    student *students_data;
    int count = 10;

    switch (argc)
    {
    case 5:
        max_total = atoi(argv[4]);
    case 4:
        max_east = atoi(argv[3]);
    case 3:
        max_west = atoi(argv[2]);
    case 2:
        count = atoi(argv[1]);
        break;
    default:
        printf("part6c [# of monkeys] [M] [N] [K]\n");
        printf("defaults: 10 3 3 5\n");
        break;
    }

    students = malloc(count*sizeof(pthread_t));
    students_data = malloc(count*sizeof(student));

    printf("Simulation started.\n");

    for (int i = 0; i < count; i++) {
        students_data[i].id = i;
        students_data[i].dest = rand() % 2;

        pthread_create(&students[i], NULL, student_work, &students_data[i]);

        sleep(1);
    }

    while (completions < count) {
        sleep(1);
    }

    printf("Simulation ended.\n");

    free(students);
    free(students_data);
}