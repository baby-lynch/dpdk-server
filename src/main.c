#include <arpa/inet.h>
#include <stdio.h>

#include "context.h"
#include "kni.h"
#include "ringbuffer.h"
#include "seasched.h"
#include "util.h"

#define DEBUG_OUT_PACKET 0
#define DEBUG_IN_PACKET 1

static int promiscuous_on = 1; /* Port set in promiscuous mode off by default */

static const struct rte_eth_conf port_conf_default = {
    .rxmode = {
        .split_hdr_size = 0,
    },
    .txmode = {
        .mq_mode = RTE_ETH_MQ_TX_NONE,
    },
};

#if 0
static inline int
port_init(uint16_t port) {
    int retval;
    struct rte_eth_conf port_conf = port_conf_default;

    if (!rte_eth_dev_is_valid_port(port)) {
        return -1;
    }

    struct rte_eth_dev_info dev_info;
    retval = rte_eth_dev_info_get(port, &dev_info);
    if (retval != 0) {
        printf("Error during getting device (port %u) info: %s\n", port, strerror(-retval));
        return retval;
    }

    /* Configure the Ethernet device */
    const uint16_t nb_rx_queue = 1;
    const uint16_t nb_tx_queue = 1;
    uint16_t nb_rx_desc = RX_RING_SIZE;
    uint16_t nb_tx_desc = TX_RING_SIZE;
    retval = rte_eth_dev_configure(port, nb_rx_queue, nb_tx_queue, &port_conf);
    if (retval != 0) {
        return retval;
    }
    retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rx_desc, &nb_tx_desc);
    if (retval != 0) {
        return retval;
    }

    /* Allocate and set up 1 RX queue per Ethernet port */
    uint16_t queue_id;
    for (queue_id = 0; queue_id < nb_rx_queue; queue_id++) {
        retval = rte_eth_rx_queue_setup(port, queue_id, nb_rx_desc,
                                        rte_eth_dev_socket_id(port), NULL, mbuf_pool_get());
        if (retval < 0) {
            return retval;
        }
    }

    /* Allocate and set up 1 TX queue per Ethernet port */
    struct rte_eth_txconf txconf;
    txconf = dev_info.default_txconf;
    txconf.offloads = port_conf.txmode.offloads;
    for (queue_id = 0; queue_id < nb_tx_queue; queue_id++) {
        retval = rte_eth_tx_queue_setup(port, queue_id, nb_tx_desc,
                                        rte_eth_dev_socket_id(port), &txconf);
        if (retval < 0) {
            return retval;
        }
    }

    /* Start the Ethernet port */
    retval = rte_eth_dev_start(port);
    if (retval < 0) {
        return retval;
    }

    /* Display the port MAC address */
    struct rte_ether_addr addr;
    retval = rte_eth_macaddr_get(port, &addr);
    if (retval != 0) {
        return retval;
    }
    printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
           " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
           port,
           addr.addr_bytes[0], addr.addr_bytes[1],
           addr.addr_bytes[2], addr.addr_bytes[3],
           addr.addr_bytes[4], addr.addr_bytes[5]);

    /* Enable RX in promiscuous mode if promicuous mode on */
    if (promiscuous_on) {
        retval = rte_eth_promiscuous_enable(port);
        if (retval != 0) {
            return retval;
        }
    }
    return 0;
}
#endif

static inline int
port_init(uint16_t port) {
    struct rte_eth_conf port_conf;
    const uint16_t rx_rings = 1, tx_rings = 1;
    uint16_t nb_rxd = RX_RING_SIZE;
    uint16_t nb_txd = TX_RING_SIZE;
    int retval;
    uint16_t q;
    struct rte_eth_dev_info dev_info;
    struct rte_eth_txconf txconf;

    if (!rte_eth_dev_is_valid_port(port))
        return -1;

    memset(&port_conf, 0, sizeof(struct rte_eth_conf));

    retval = rte_eth_dev_info_get(port, &dev_info);
    if (retval != 0) {
        printf("Error during getting device (port %u) info: %s\n",
               port, strerror(-retval));
        return retval;
    }

    if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE)
        port_conf.txmode.offloads |=
            RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE;

    /* Configure the Ethernet device. */
    retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
    if (retval != 0)
        return retval;

    retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
    if (retval != 0)
        return retval;

    /* Allocate and set up 1 RX queue per Ethernet port. */
    for (q = 0; q < rx_rings; q++) {
        retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
                                        rte_eth_dev_socket_id(port), NULL, mbuf_pool_get());
        if (retval < 0)
            return retval;
    }

    txconf = dev_info.default_txconf;
    txconf.offloads = port_conf.txmode.offloads;
    /* Allocate and set up 1 TX queue per Ethernet port. */
    for (q = 0; q < tx_rings; q++) {
        retval = rte_eth_tx_queue_setup(port, q, nb_txd,
                                        rte_eth_dev_socket_id(port), &txconf);
        if (retval < 0)
            return retval;
    }

    /* Starting Ethernet port. 8< */
    retval = rte_eth_dev_start(port);
    /* >8 End of starting of ethernet port. */
    if (retval < 0)
        return retval;

    /* Display the port MAC address. */
    struct rte_ether_addr addr;
    retval = rte_eth_macaddr_get(port, &addr);
    if (retval != 0)
        return retval;

    /* Enable RX in promiscuous mode for the Ethernet device. */
    retval = rte_eth_promiscuous_enable(port);
    /* End of setting RX port in promiscuous mode. */
    if (retval != 0)
        return retval;

    return 0;
}

/*
 * lcore_rx_action do the following jobs:
 * Dequeue packets from ringbuffer->in and deliver them to corresponding processing stack through kni；
 */
static __attribute__((noreturn)) int lcore_rx_action(__attribute__((unused)) void *arg) {
    printf("[ rx_thread ] is runing on slave core: %u\n ", rte_lcore_id());
    while (1) {
        struct rte_mbuf *mbufs[BURST_SIZE];
        uint16_t nr_recvd = rte_ring_sc_dequeue_burst(ring_buffer_get()->in, (void **)mbufs, BURST_SIZE, NULL);
        uint16_t i = 0;
        for (; i < nr_recvd; i++) {
#if DEBUG_IN_PACKET
            dump_packet(mbufs[i]);
#endif
            struct rte_ether_hdr *ethdr = rte_pktmbuf_mtod(mbufs[i], struct rte_ether_hdr *);
            switch (rte_be_to_cpu_16(ethdr->ether_type)) {
                case RTE_ETHER_TYPE_IPV6: {
                    rte_ring_sp_enqueue(sched_ring_get(), (void *)mbufs[i]);
                    struct rte_ipv6_hdr *ip6hdr = rte_pktmbuf_mtod_offset(mbufs[i], struct rte_ipv6_hdr *, RTE_ETHER_HDR_LEN);
                    if (ip6hdr->proto == 0x99) {
                        printf("seadp packet\n");
                        // rte_ring_sp_enqueue(sched_ring_get(), (void *)mbufs[i]);
                    } else {
                        rte_kni_tx_burst(kni_get(), &mbufs[i], 1);
                    }
                    break;
                }
                default: {
                    rte_kni_tx_burst(kni_get(), &mbufs[i], 1);
                    break;
                }
            }
            rte_pktmbuf_free(mbufs[i]);
        }
    }
}

/*
 * lcore_rx_action do the following jobs:
 * Receive packets from kni and enqueue them to ringbuffer->out
 */
static __attribute__((noreturn)) int lcore_tx_action(__attribute__((unused)) void *arg) {
    printf("[ tx_thread ] is runing on slave core: %u\n ", rte_lcore_id());
    while (1) {
        rte_kni_handle_request(kni_get());

        struct rte_mbuf *mbufs[BURST_SIZE];
        uint16_t kni_nb_rx = rte_kni_rx_burst(kni_get(), mbufs, BURST_SIZE);
        if (unlikely(kni_nb_rx > BURST_SIZE)) {
            rte_exit(EXIT_FAILURE, "too many packets, %d", kni_nb_rx);
        }

#if DEBUG_OUT_PACKET
        if (kni_nb_rx > 0) {
            for (int i = 0; i < kni_nb_rx; ++i) {
                struct rte_ether_hdr *ehdr = rte_pktmbuf_mtod(mbufs[i], struct rte_ether_hdr *);

                dump_packet(mbufs[i]);
                if (ehdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
                    struct rte_ipv4_hdr *iphdr = (struct rte_ipv4_hdr *)(ehdr + 1);
                    struct in_addr addr;
                    addr.s_addr = iphdr->dst_addr;
                    printf("kni out destination: %s ", inet_ntoa(addr));
                }
            }
        }
#endif

        int kni_nb_tx = rte_ring_sp_enqueue_burst(ring_buffer_get()->out, (void **)mbufs, kni_nb_rx, NULL);

        for (int i = kni_nb_tx; i < kni_nb_rx; ++i) {
            rte_pktmbuf_free(mbufs[i]);
        }
    }
}

/*
 * lcore_rx_schedule do the following jobs:
 * Deque packets from sched_ring and schedule them into priority queue
 */
static __attribute__((noreturn)) int lcore_schedule(__attribute__((unused)) void *arg) {
    printf("[ schedule_thread ] is runing on slave core: %u\n ", rte_lcore_id());
    while (1) {
        struct rte_mbuf *mbufs[BURST_SIZE];
        uint16_t nr_recvd = rte_ring_sc_dequeue_burst(sched_ring_get(), (void **)mbufs, BURST_SIZE, NULL);
        uint16_t i = 0;
        for (; i < nr_recvd; i++) {
            if (0 != schedule(mbufs[i])) {
                continue;
            }
        }
    }
}

/*
 * lcore_main do the following jobs:
 * 1.Retrieve packets from NIC rx queue and enqueue them to Ring Buffer
 *   Packet in flow: rx queue ---> rx_mbufs ---> ringbuffer->in
 * 2.Dequeue packets from Ring Buffer and push them to NIC tx queue
 *   Packet out flow: ringbuffer->out ---> tx_mbufs ---> tx queue
 */
static __attribute__((noreturn)) void lcore_main(void) {
    uint16_t lcore_id = rte_get_main_lcore();
    printf("[ main thread ] is runing on master core: %u\n ", lcore_id);

    /* Launch a slave core for packet receiving */
    lcore_id = rte_get_next_lcore(lcore_id, 1, 0);
    rte_eal_remote_launch(lcore_rx_action, NULL, rte_get_next_lcore(lcore_id, 1, 0));

    /* Launch a slave core for packet sending */
    lcore_id = rte_get_next_lcore(lcore_id, 1, 0);
    rte_eal_remote_launch(lcore_tx_action, NULL, rte_get_next_lcore(lcore_id, 1, 0));

    /* Launch a slave core for packet scheduling */
    lcore_id = rte_get_next_lcore(lcore_id, 1, 0);
    rte_eal_remote_launch(lcore_schedule, NULL, rte_get_next_lcore(lcore_id, 1, 0));

    while (1) {
        // RX
        uint16_t nr_recvd;
        struct rte_mbuf *rx_mbufs[BURST_SIZE];
        nr_recvd = rte_eth_rx_burst(ETH_DEV_PORT_ID, RX_QUEUE_ID, rx_mbufs, BURST_SIZE);
        if (unlikely(nr_recvd > BURST_SIZE)) {
            rte_exit(EXIT_FAILURE, "too many packets, %d", nr_recvd);
        }
        if (nr_recvd > 0) {
            rte_ring_sp_enqueue_burst(ring_buffer_get()->in, (void **)rx_mbufs, nr_recvd, NULL);
        }

        // TX
        uint16_t nr_send;
        struct rte_mbuf *tx_mbufs[BURST_SIZE];
        nr_send = rte_ring_sc_dequeue_burst(ring_buffer_get()->out, (void **)tx_mbufs, BURST_SIZE, NULL);
        if (nr_send > 0) {
            rte_eth_tx_burst(ETH_DEV_PORT_ID, TX_QUEUE_ID, tx_mbufs, nr_send);
            /* traffic_policer for traffic shaping */
            for (uint16_t i = 0; i < nr_send; i++) {
                rte_pktmbuf_free(tx_mbufs[i]);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    /* Check DPDK env config */
    if (rte_eal_init(argc, argv) < 0) {
        rte_exit(EXIT_FAILURE, "Error with eal init\n");
    }

    /* Initialize mempool */
    mbuf_pool_init();
    if (mbuf_pool_get() == NULL) {
        rte_exit(EXIT_FAILURE, "Mempool create failed\n");
    }

    /* Initialize port */
    if (port_init(ETH_DEV_PORT_ID) != 0) {
        rte_exit(EXIT_FAILURE, "Cannot init port %" PRIu16 "\n", ETH_DEV_PORT_ID);
    }

    /* Initialize ring buffer */
    ring_buffer_init();
    if (ring_buffer_get() == NULL) {
        rte_exit(EXIT_FAILURE, "ring buffer init failed\n");
    }

    /* Initialize kni */
    kni_init();
    if (kni_get() == NULL) {
        rte_exit(EXIT_FAILURE, "kni alloc failed\n");
    }

    /* Initialize priority queue */
    init_pqueue();
    if (pqueue_get() == NULL) {
        rte_exit(EXIT_FAILURE, "priority queue init failed\n");
    }

    /* Initialize scheduler ring */
    sched_ring_init();
    if (sched_ring_get() == NULL) {
        rte_exit(EXIT_FAILURE, "sched ring init failed\n");
    }

    /* Initialize begin time */
    begin_time_init();

    /* Call lcore main */
    lcore_main();

    /* Clean up the EAL */
    rte_eal_cleanup();
}