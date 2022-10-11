#ifndef _SEASCHED_H_
#define _SEASCHED_H_

#include <rte_ring.h>

#include "node.h"
#include "pqueue.h"

struct seanet_hdr {
    uint8_t id_next_head_type;
    uint8_t id_length;
    uint16_t id_seanet_prot_prop;
    char id_src_eid[20];
    char id_dst_eid[20];
} __attribute__((__packed__));

struct seadp_hdr {
    uint8_t pflag_type;
    uint8_t hdr_len;
    uint8_t cflag;
    uint8_t sflag;
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t pn;
    uint16_t cksum;
    uint32_t offset;
    uint32_t len;
} __attribute__((__packed__));

/* create a scheduler ring */
struct rte_ring* sched_ring_create(const char* name, int ring_size);

/* schedule a request packet, i.e. put a packet into priority queue
   return 0 on success, -1 on fail
 */
int schedule(void* arg);

#endif