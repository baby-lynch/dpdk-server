#ifndef KNI_H
#define KNI_H

#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <rte_arp.h>
#include <rte_ethdev.h>
#include <rte_icmp.h>
#include <rte_ip.h>
#include <rte_kni.h>
#include <rte_tcp.h>
#include <rte_udp.h>

#include "context.h"
#include "debug.h"

#define MAX_MBUF_SIZE 4096

#define PKT_BURST_SIZE 32

/* Initialize rte_kni_conf and rte_kni_ops,
 * and create a kni interface dynamically via the rte_kni_alloc() function
 */
struct rte_kni *kni_alloc(struct rte_mempool *mbuf_pool, uint16_t port_id);

#endif