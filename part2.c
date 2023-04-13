/*
Question 2 of OS Homework 5 by Chris Lee

This program implements the cigarettes smokers problem.
=============================================
Agent thread algorithm:
=============================================
while true:
    pick random ingredients to serve
    serve ingredients
    wait until needed to serve again
=============================================
=============================================
Smoker thread algorithm:
=============================================
while true:
    wait to grab first needed ingredient
    if can grab second needed ingredient:
        smoke
        tell agent to serve again
    else:
        drop first needed ingredient
=============================================
*/

#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

sem_t tobacco_sem;
sem_t paper_sem;
sem_t match_sem;

sem_t more_needed_alarm;

enum INGREDIENTS {
    TOBACCO=0,
    PAPER,
    MATCHES
};

void has_tobacco() {
}

void *smoker_work(void *vinv) {
    unsigned long long inv = (unsigned long long)vinv;
    sem_t *needed1;
    sem_t *needed2;
    char *name;

    switch (inv)
    {
    case TOBACCO:
        needed1 = &paper_sem;
        needed2 = &match_sem;
        name = "Tobacco";
        break;
    case PAPER:
        needed1 = &tobacco_sem;
        needed2 = &match_sem;
        name = "Paper";
        break;
    case MATCHES:
        needed1 = &tobacco_sem;
        needed2 = &paper_sem;
        name = "Matches";
        break;
    default:
        exit(-1);
    }

    while (1) {
        sem_wait(needed1); // grab first necessary ingredient
        if (sem_trywait(needed2) == 0) { // make sure second ingredient available
            printf("Smoker %s is smoking now.\n", name);
            sem_post(&more_needed_alarm);
        } else { // if not then drop
            sem_post(needed1);
        }
    }
}

void *agent_work(void *viter) {
    srand(time(NULL));

    int iterations = (unsigned long long)viter;
    if (iterations == 0) {
        iterations = INT_MAX;
    }

    while (iterations > 0) {
        int r_ingredients = rand() % 3;

        switch (r_ingredients)
        {
        case 0: // for tobacco haver
            printf("Agent dispensing paper and matches now.\n");
            sem_post(&paper_sem);
            sem_post(&match_sem);
            break;
        case 1: // for paper haver
            printf("Agent dispensing tobacco and matches now.\n");
            sem_post(&tobacco_sem);
            sem_post(&match_sem);
            break;
        case 2: // for matches haver
            printf("Agent dispensing tobacco and paper now.\n");
            sem_post(&tobacco_sem);
            sem_post(&paper_sem);
            break;
        default:
            break;
        }
        sem_wait(&more_needed_alarm);
        iterations--;
    }
}

int main(int argc, char **argv) {
    pthread_t agent;
    pthread_t smokers[3];

    sem_init(&tobacco_sem, 0, 0);
    sem_init(&paper_sem, 0, 0);
    sem_init(&match_sem, 0, 0);
    sem_init(&more_needed_alarm, 0, 0);

    unsigned long long iterations_to_run = 5;

    if (argc == 2) {
        iterations_to_run = atoi(argv[1]);
    } else {
        printf("usage: part2 [iterations]\n");
        printf("defaults: 5");
    }

    printf("Starting simulation now for %u iterations.\n", iterations_to_run);

    pthread_create(&agent, NULL, agent_work, (void*)iterations_to_run);
    
    pthread_create(&smokers[TOBACCO], NULL, smoker_work, (void*)(unsigned long long)TOBACCO);
    pthread_create(&smokers[PAPER], NULL, smoker_work, (void*)(unsigned long long)PAPER);
    pthread_create(&smokers[MATCHES], NULL, smoker_work, (void*)(unsigned long long)MATCHES);

    pthread_join(agent, NULL);

    return 0;
}