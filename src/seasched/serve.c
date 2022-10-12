#include "serve.h"

int serve(void *arg) {
    struct node *pkt_node = (struct node *)arg;
    print_node(pkt_node);
    struct rte_mbuf *pkt = pkt_node->data;
    rte_ring_enqueue(ring_buffer_get()->out, pkt);
}