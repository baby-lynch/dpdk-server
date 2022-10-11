#ifndef CONTEXT_H
#define CONTEXT_H

#include <assert.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <rte_ethdev.h>
#include <stdio.h>
#include <stdlib.h>

#include "kni.h"
#include "ringbuffer.h"
#include "seasched.h"
#include "util.h"

#define ETH_DEV_PORT_ID 0 /* specified port: port0 (ens162) */

#define RX_QUEUE_ID 0 /* use one rx queue only */
#define TX_QUEUE_ID 0 /* use one tx queue only */

#define RX_RING_SIZE 1024 /* Number of RX ring descriptors */
#define TX_RING_SIZE 1024 /* Number of TX ring descriptors */

#define NUM_BUFS (8 * 1024 - 1) /* Number of mbufs in mempool that is created */
#define MBUF_CACHE_SIZE 250

#define BURST_SIZE 32
#define RING_BUF_SIZE 64

#define PQUEUE_GROWING_STEP 200 /* Growing Step of Priority Queue */

/* Global Initialization of mempool */
void mbuf_pool_init(void);
/* Return the pointer to mempool */
struct rte_mempool* mbuf_pool_get(void);

/* Global Initialization of ring buffer */
void ring_buffer_init(void);
/* Return the pointer to ring buffer */
struct ring_buffer* ring_buffer_get(void);

/* Global Initialization of kni */
void kni_init(void);
/* Return the pointer to kni */
struct rte_kni* kni_get(void);

/* Global Initialization of priority queue */
void init_pqueue(void);
/* Return the pointer to priority queue */
pqueue_t* pqueue_get(void);

/* Global Initialization of sched ring */
void sched_ring_init(void);
/* Return the pointer to sched ring */
struct rte_ring* sched_ring_get(void);

/* Global Initialization of begin time */
void begin_time_init(void);
/* Return begin time */
double begin_time_get(void);
#endif