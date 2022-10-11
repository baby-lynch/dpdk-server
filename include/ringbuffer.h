#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <rte_malloc.h>
#include <rte_mbuf.h>
#include <rte_ring.h>
#include <string.h>

struct ring_buffer {
    struct rte_ring *in;
    struct rte_ring *out;
};

/* Create a ring buffer
 * @names: name of ring_buffer->in and ring_buffer->out
 * @ring_size: size of ring_buffer->in and ring_buffer->out
 */
struct ring_buffer *ring_buffer_create(const char *names[2], int ring_size);

#endif
