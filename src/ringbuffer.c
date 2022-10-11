#include "ringbuffer.h"

/* Create ring buffer */
struct ring_buffer *ring_buffer_create(const char *names[2], int ring_size) {
    struct ring_buffer *rb = NULL;
    rb = rte_malloc("ring buffer", sizeof(struct ring_buffer), 0);
    memset(rb, 0, sizeof(struct ring_buffer));

    rb->in = rte_ring_create(names[0], ring_size, rte_socket_id(),
                             RING_F_SP_ENQ | RING_F_SC_DEQ);
    rb->out = rte_ring_create(names[1], ring_size, rte_socket_id(),
                              RING_F_SP_ENQ | RING_F_SC_DEQ);
    return rb;
}