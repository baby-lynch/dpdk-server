#ifndef _NODE_H_
#define _NODE_H_

#include <stdio.h>
#include <stdlib.h>

#include "pqueue.h"

/*A wrapper for request packets to be inserted into priority queue */
struct node {
    pqueue_pri_t pri;
    size_t pos;
    double arrive_time;
    double cost;
    void *data;
};

int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr);

pqueue_pri_t get_pri(void *a);

void set_pri(void *a, pqueue_pri_t pri);

size_t get_pos(void *a);

void set_pos(void *a, size_t pos);

void print_node(FILE *out, void *a);
#endif
