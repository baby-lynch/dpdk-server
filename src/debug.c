#include "debug.h"

void dump_packet(void *arg) {
    struct rte_mbuf *pkt = (struct rte_mbuf *)arg;
    struct rte_ether_hdr *ethdr = rte_pktmbuf_mtod(pkt, struct rte_ether_hdr *);
    switch (rte_be_to_cpu_16(ethdr->ether_type)) {
        case RTE_ETHER_TYPE_ARP: {
            // dump_arp_packet(pkt);
            break;
        }
        case RTE_ETHER_TYPE_IPV4: {
            IPPROTO_IGMP;
            // dump_ipv4_packet(pkt);
            break;
        }
        case RTE_ETHER_TYPE_IPV6: {
            dump_ipv6_packet(pkt);
            break;
        }
        default:
            printf("\nOther Ethernet frame, frame type: 0x%02x\n", rte_be_to_cpu_16(ethdr->ether_type));
    }
}

/* ARP
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Hardware Type       |         Protocol Type         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Hard Addr Len| Prot Addr Len |            Operation          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                   Sender Hardware Address                   |
|                             +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             |       Sender IP Address       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                   Target Hardware Address                   |
|                             +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             |       Target IP Address       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|          Identifier         |         Sequence Number       |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
void dump_arp_packet(void *arg) {
    struct rte_mbuf *pkt = (struct rte_mbuf *)arg;
    struct rte_arp_hdr *arphdr = rte_pktmbuf_mtod_offset(pkt, struct rte_arp_hdr *, RTE_ETHER_HDR_LEN);

    struct rte_ether_addr sha, tha;
    memcpy(&sha, &arphdr->arp_data.arp_sha, RTE_ETHER_ADDR_LEN);
    memcpy(&tha, &arphdr->arp_data.arp_tha, RTE_ETHER_ADDR_LEN);

    struct in_addr sip, tip;
    sip.s_addr = arphdr->arp_data.arp_sip;
    tip.s_addr = arphdr->arp_data.arp_tip;

    printf("\n++++++++++++ ARP Packet ++++++++++++\n");
    printf("| Hardware Type: %02x\n", ntohs(arphdr->arp_hardware));
    printf("| Protocol Type: %02x\n", ntohs(arphdr->arp_protocol));
    printf("| Hardware Address len: %d\n", arphdr->arp_hlen);
    printf("| Protocol Address len: %d\n", arphdr->arp_plen);
    printf("| Opetation Code: %d (Request:1, Reply:2 )\n", ntohs(arphdr->arp_opcode));
    printf("| Sender Hardware Address: %02x %02x %02x %02x %02x %02x\n",
           sha.addr_bytes[0], sha.addr_bytes[1], sha.addr_bytes[2],
           sha.addr_bytes[3], sha.addr_bytes[4], sha.addr_bytes[5]);
    printf("| Sender IP:%s\n", inet_ntoa(sip));

    printf("| Target Hardware Address: %02x %02x %02x %02x %02x %02x\n",
           tha.addr_bytes[0], tha.addr_bytes[1], tha.addr_bytes[2],
           tha.addr_bytes[3], tha.addr_bytes[4], tha.addr_bytes[5]);
    printf("| Target IP:%s\n", inet_ntoa(tip));
    printf("+++++++++++++++++++++++++++++++++++++++\n");
}

/* IPv4
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| Ver |  IHL  |Type of Service|           Total Length        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|         Identification      |Flags|       Fragment Offset   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| Time to Live|    Protocol   |          Header Checksum      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       Source Address                        |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                    Destination Address                      |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
void dump_ipv4_packet(void *arg) {
    struct rte_mbuf *pkt = (struct rte_mbuf *)arg;
    struct rte_ipv4_hdr *iphdr = rte_pktmbuf_mtod_offset(pkt, struct rte_ipv4_hdr *, RTE_ETHER_HDR_LEN);
    struct in_addr src_addr, dst_addr;
    src_addr.s_addr = iphdr->src_addr;
    dst_addr.s_addr = iphdr->dst_addr;
    printf("\n++++++++++++ IPv4 Packet ++++++++++++\n");
    printf("| Version:%d\n", ((iphdr->version_ihl) & 0xf0) >> 4);
    printf("| Header Length:%d\n", ((iphdr->version_ihl) & 0x0f));
    printf("| ToS:%02x\n", iphdr->type_of_service);
    printf("| Total Length:%d\n", iphdr->total_length);
    printf("| Identification:%02x\n", iphdr->packet_id);
    printf("| Fragment:%02x\n", iphdr->fragment_offset);
    printf("| TTL:%d\n", iphdr->time_to_live);
    printf("| Protocol:%d\n", iphdr->next_proto_id);
    printf("| Header Checksum:%02x\n", iphdr->hdr_checksum);
    printf("| Source IP:%s\n", inet_ntoa(src_addr));
    printf("| Destination IP:%s\n", inet_ntoa(dst_addr));
    printf("+++++++++++++++++++++++++++++++++++++++\n");
}

/* IPv6
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| Ver | Traffic Class |             Flow Label                |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|        Payload Length       |  Next Header  |    Hop Limit  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                              |
+                                                              +
|                                                              |
+                         Source Address                       +
|                                                              |
+                                                              +
|                                                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                              |
+                                                              +
|                                                              |
+                      Destination Address                     +
|                                                              |
+                                                              +
|                                                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
void dump_ipv6_packet(void *arg) {
    struct rte_mbuf *pkt = (struct rte_mbuf *)arg;
    struct rte_ipv6_hdr *ip6hdr = rte_pktmbuf_mtod_offset(pkt, struct rte_ipv6_hdr *, RTE_ETHER_HDR_LEN);
    char src_addr[20] = {0};
    char dst_addr[20] = {0};
    inet_ntop(AF_INET6, ip6hdr->src_addr, src_addr, sizeof(src_addr));
    inet_ntop(AF_INET6, ip6hdr->dst_addr, dst_addr, sizeof(dst_addr));

    printf("\n++++++++++++ IPv6 Packet ++++++++++++\n");
    printf("| Version:%d\n", (rte_be_to_cpu_32(ip6hdr->vtc_flow) & 0xf0000000) >> 28);
    printf("| Traffic Class:%d\n", (rte_be_to_cpu_32(ip6hdr->vtc_flow) & 0xff00000) >> 20);
    printf("| Flow Table:0x%02x\n", rte_be_to_cpu_32(ip6hdr->vtc_flow) & 0xfffff);
    printf("| Payload Length:%d\n", rte_be_to_cpu_16(ip6hdr->payload_len));
    printf("| Next Header:%d\n", ip6hdr->proto);
    printf("| Hop Limit:%d\n", ip6hdr->hop_limits);
    printf("| Source IP:%s\n", src_addr);
    printf("| Destination IP:%s\n", dst_addr);
    printf("+++++++++++++++++++++++++++++++++++++++\n");
}
