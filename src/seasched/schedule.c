#include "schedule.h"

static double ALPHA = 2.0;

struct rte_ring *sched_ring_create(const char *name, int ring_size) {
    struct rte_ring *r = rte_ring_create(name, ring_size, rte_socket_id(),
                                         RING_F_SP_ENQ | RING_F_SC_DEQ);
    return r;
}

/* calculate cost of a request packet */
static double calc_cost(void *arg) {
    // simply take the size of packet payload as cost, cuz this is a echo server
    struct rte_mbuf *pkt = (struct rte_mbuf *)arg;
    return pkt->data_len;
}

/* wrap a request packet into node struct */
static struct node *
wrap_pkt(void *arg) {
    struct rte_mbuf *pkt = (struct rte_mbuf *)arg;
    struct node *nd = rte_malloc(NULL, sizeof(struct node), 0);
    nd->arrive_time = timestamp() - begin_time_get();
    nd->cost = calc_cost(pkt);
    nd->pri = nd->arrive_time + ALPHA * nd->cost;
    nd->data = pkt;
    return nd;
}

int schedule(void *arg) {
    struct rte_mbuf *pkt = (struct rte_mbuf *)arg;
    struct node *pkt_node = wrap_pkt(pkt);

    if (0 != pqueue_insert(pqueue_get(), pkt_node)) {
        printf("packet enter priority queue failed !");
        return -1;
    }
    return 0;
}