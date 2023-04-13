#include "queue.h"

queue queue_create(int n) {
    queue n_queue;
    n_queue.data = malloc(n*sizeof(char));
    memset(n_queue.data, 0, n*sizeof(char));
    n_queue.size = 0;
    n_queue.n = n;

    return n_queue;
}

char queue_pop(queue *c) {
    char popped = c->data[0];
    for (int i = 1; i < c->n; i++) {
        c->data[i-1] = c->data[i];
    }
    c->data[c->n - 1] = 0;
    c->size -= 1;

    return popped;
}

char queue_peek(queue *c) {
    return c->data[0];
}

int queue_empty(queue *c) {
    return c->size == 0;
}

int queue_full(queue *c) {
    return c->size == c->n;
}

int queue_add(queue *c, char ch) {
    if (c->size == c->n) {
        return 0;
    }

    c->data[c->size] = ch;
    c->size += 1;

    return 1;
}