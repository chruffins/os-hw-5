#ifndef OS_QUEUE_H
#define OS_QUEUE_H

#include<stdlib.h>
#include<string.h>

struct queue_t {
    char *data;
    int n;
    int size;
} typedef queue;

queue queue_create(int n);
char queue_pop(queue *c);
char queue_peek(queue *c);
int queue_empty(queue *c);
int queue_full(queue *c);
int queue_add(queue *c, char ch);

#endif