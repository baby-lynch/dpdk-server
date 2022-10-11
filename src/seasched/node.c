#include "node.h"

int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr) {
    return (next > curr);
}

pqueue_pri_t get_pri(void *a) {
    return ((struct node *)a)->pri;
}
void set_pri(void *a, pqueue_pri_t pri) {
    ((struct node *)a)->pri = pri;
}

size_t get_pos(void *a) {
    return ((struct node *)a)->pos;
}

void set_pos(void *a, size_t pos) {
    ((struct node *)a)->pos = pos;
}

void print_node(FILE *out, void *a) {
    struct node *nd = (struct node *)a;
    printf("\n+++++++++++++++ Node +++++++++++++++\n");
    printf("| Arrive Time: %lf\n", nd->arrive_time);
    printf("| Cost: %lf\n", nd->cost);
    printf("| Priority: %lf\n", nd->pri);
    printf("++++++++++++++++++++++++++++++++++++\n");

    // fprintf(out, "\n+++++++++++++++ Node +++++++++++++++\n");
    // fprintf(out, "| Arrive Time: %lf", nd->arrive_time);
    // fprintf(out, "| Cost: %lf", nd->cost);
    // fprintf(out, "| Priority: %lf", nd->pri);
    // fprintf(out, "++++++++++++++++++++++++++++++++++++\n");
}