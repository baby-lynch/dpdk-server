#ifndef DEGUB_H
#define DEBUG_H

#include <arpa/inet.h>
#include <rte_arp.h>
#include <rte_ethdev.h>
#include <rte_icmp.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>

#include "seasched.h"

/*--------------------- debug in/out packet ---------------------*/
void dump_packet(void *arg);
void dump_arp_packet(void *arg);
void dump_ipv4_packet(void *arg);
void dump_ipv6_packet(void *arg);

/*--------------------- debug priority queue ---------------------*/

#endif