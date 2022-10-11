#include "context.h"

static struct rte_mempool *global_mbuf_pool = NULL;   /* mempool is here defined globally */
static struct ring_buffer *global_ring_buffer = NULL; /* ring buffer is here defined globally */
static struct rte_kni *global_kni = NULL;             /* kni is here defined globally */
static pqueue_t *global_pq = NULL;                    /* priority queue is here defined globally */
static struct rte_ring *global_sched_ring = NULL;     /* scheduler ring is here defined globally */
static double begin_time = 0;                         /* begin time */

/*----------- Mempool -----------*/
void mbuf_pool_init(void) {
    global_mbuf_pool = rte_pktmbuf_pool_create("mbuf pool", NUM_BUFS,
                                               MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE,
                                               rte_socket_id());
}
struct rte_mempool *mbuf_pool_get(void) {
    assert(global_mbuf_pool);
    return global_mbuf_pool;
}

/*----------- Ring Buffer -----------*/
void ring_buffer_init(void) {
    const char *names[2] = {"in ring", "out ring"};
    global_ring_buffer = ring_buffer_create(names, RING_BUF_SIZE);
}
struct ring_buffer *ring_buffer_get(void) {
    assert(global_ring_buffer);
    return global_ring_buffer;
}

/*----------- KNI -----------*/
void kni_init(void) {
    if (rte_kni_init(1) < 0) {
        rte_exit(EXIT_FAILURE, "Cannot init kni interface\n");
    }
    global_kni = kni_alloc(mbuf_pool_get(), ETH_DEV_PORT_ID);
}
struct rte_kni *kni_get(void) {
    assert(global_kni);
    return global_kni;
}

/*----------- Priority Queue -----------*/
void init_pqueue(void) {
    uint16_t pq_initial_len = PQUEUE_GROWING_STEP * 10;
    pqueue_t *pq = pqueue_init(pq_initial_len, cmp_pri, get_pri, set_pri, get_pos, set_pos);
    if (!pq) {
        rte_exit(EXIT_FAILURE, "Cannot init priority queue\n");
    }
    global_pq = pq;

    /*init output file of pqueue */
    FILE *pq_out = NULL;
    if ((pq_out = fopen("ppqq_out.txt", "w+")) == NULL) {
        printf("Cannot open output file\n");
    }
    fprintf(pq_out,
            "**************************************\
                        Priority Queue\
            **************************************\n");
}

pqueue_t *pqueue_get(void) {
    assert(global_pq);
    return global_pq;
}

/*----------- Scheduler Ring -----------*/
void sched_ring_init(void) {
    const char *name = "scheduler ring";
    global_sched_ring = sched_ring_create(name, RING_BUF_SIZE);
}
struct rte_ring *sched_ring_get(void) {
    assert(global_sched_ring);
    return global_sched_ring;
}

/*----------- Begin Time -----------*/
void begin_time_init(void) {
    begin_time = timestamp();
    printf("begin time: %lf s\n", begin_time);
}
double begin_time_get(void) {
    return begin_time;
}
