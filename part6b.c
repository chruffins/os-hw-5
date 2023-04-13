/*
Question 6b of OS Homework 5 by Chris Lee

This program implements the starvation prevention
version of the monkey crossing problem.
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

struct os_monkey_t {
    int id;
    DESTINATION dest;
} typedef monkey;

void wait_until_safe_to_cross(DESTINATION d);
void cross_ravine(int id, DESTINATION d);
void done_with_crossing(DESTINATION d);

signed char direction;
signed char monkeys_on_rope;
unsigned char streak;

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
        if ((direction == -1 || direction == 0) && monkeys_on_rope < 5 && streak < 5) {
            direction = -1;
            monkeys_on_rope++;
            pthread_mutex_unlock(&rope_mutex);
            return;
        } else {
            pthread_mutex_unlock(&rope_mutex);
            sem_wait(&safe_for_west);
        }
        break;
    case EAST:
        pthread_mutex_lock(&rope_mutex);
        if ((direction == 1 || direction == 0) && monkeys_on_rope < 5 && streak < 5) {
            direction = 1;
            monkeys_on_rope++;
            pthread_mutex_unlock(&rope_mutex);
            return;
        } else {
            pthread_mutex_unlock(&rope_mutex);
            sem_wait(&safe_for_east);
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
        printf("WEST %d STARTED (%d ON ROPE NOW)\n", id, monkeys_on_rope);
        break;
    case EAST:
        printf("EAST %d STARTED (%d ON ROPE NOW)\n", id, monkeys_on_rope);
        break;
    }

    sleep(2);

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
    switch (d)
    {
    case WEST:
        pthread_mutex_lock(&rope_mutex);
        monkeys_on_rope--;
        streak++;
        if (streak == 5) {
            printf("Anti-starvation has been implemented! No more WEST monkeys can cross for now.\n");
        }
        if (monkeys_on_rope == 0) {
            streak = 0;
            direction = 0;
            printf("WESTBOUND MONKEYS FINISHED CROSSING\n");
            sem_post(&safe_for_east);
            sem_post(&safe_for_west);
        } else {
            sem_post(&safe_for_west);
        }
        completions++;
        pthread_mutex_unlock(&rope_mutex);
        break;
    case EAST:
        pthread_mutex_lock(&rope_mutex);
        monkeys_on_rope--;
        streak++;
        if (streak == 5) {
            printf("Anti-starvation has been implemented! No more EAST monkeys can cross for now.\n");
        }
        if (monkeys_on_rope == 0) {
            streak = 0;
            direction = 0;
            printf("EASTBOUND MONKEYS FINISHED CROSSING\n");
            sem_post(&safe_for_west);
            sem_post(&safe_for_east);
        } else {
            sem_post(&safe_for_east);
        }
        completions++;
        pthread_mutex_unlock(&rope_mutex);
        break;
    default:
        break;
    }
}

void *monkey_work(void *vself) {
    monkey *self = (monkey*)vself;

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
    monkeys_on_rope = 0;

    sem_init(&safe_for_west, 0, 0);
    sem_init(&safe_for_east, 0, 0);
    pthread_mutex_init(&rope_mutex, NULL);

    pthread_t *monkeys;
    monkey *monkeys_data;
    int count = 10;

    if (argc == 2) {
        count = atoi(argv[1]);
    }

    monkeys = malloc(count*sizeof(pthread_t));
    monkeys_data = malloc(count*sizeof(monkey));

    printf("Simulation started.\n");

    for (int i = 0; i < count; i++) {
        monkeys_data[i].id = i;
        monkeys_data[i].dest = rand() % 2;

        pthread_create(&monkeys[i], NULL, monkey_work, &monkeys_data[i]);

        sleep(1);
    }

    while (completions < count) {
        sleep(1);
    }

    printf("Simulation ended.\n");

    free(monkeys);
    free(monkeys_data);
}