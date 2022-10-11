#include "kni.h"

/* A config_network_if callback function for struct rte_kni_ops.  */
static int kni_config_network_if(uint16_t port_id, uint8_t if_up) {
    int ret = 0;

    if (!rte_eth_dev_is_valid_port(port_id)) {
        RTE_LOG(ERR, KNI, "Invalid port: %d\n", port_id);
        return -EINVAL;
    }
    RTE_LOG(INFO, KNI, "Configure network interface of port%d: %s\n", port_id, if_up ? "up" : "down");

    rte_eth_dev_stop(port_id);
    if (if_up) {
        ret = rte_eth_dev_start(port_id);
    }
    if (ret < 0) {
        RTE_LOG(ERR, KNI, "Failed to start port%d\n", port_id);
    }

    return ret;
}

/* A config_mac_address callback function for struct rte_kni_ops.  */
static int kni_config_mac_address(uint16_t port_id, uint8_t mac_addr[]) {
    int ret = 0;

    if (!rte_eth_dev_is_valid_port(port_id)) {
        printf("config_mac_address, Invalid port id%d\n", port_id);
        return -EINVAL;
    }

    ret = rte_eth_dev_default_mac_addr_set(port_id, (struct rte_ether_addr *)mac_addr);
    if (ret < 0) {
        printf("config_mac_address, failed to config mac_addr for port%d, mac: %s\n",
               port_id, ether_ntoa((struct ether_addr *)mac_addr));
    }

    printf("Configure mac address of port: %d, mac: %s\n",
           port_id, ether_ntoa((struct ether_addr *)(mac_addr)));

    return ret;
}

struct rte_kni *kni_alloc(struct rte_mempool *mbuf_pool, uint16_t port_id) {
    struct rte_kni_conf conf = {
        .group_id = port_id,
        .mbuf_size = MAX_MBUF_SIZE,
    };
    rte_eth_macaddr_get(port_id, (struct rte_ether_addr *)conf.mac_addr);
    rte_eth_dev_get_mtu(port_id, &conf.mtu);
    snprintf(conf.name, RTE_KNI_NAMESIZE, "kni%u", port_id);

    struct rte_kni_ops ops = {
        .port_id = port_id,
        .config_network_if = kni_config_network_if,
        .config_mac_address = kni_config_mac_address,
    };

    return rte_kni_alloc(mbuf_pool, &conf, &ops);
}
