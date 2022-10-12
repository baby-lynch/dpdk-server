#ifndef DEGUB_H
#define DEBUG_H

#include <arpa/inet.h>
#include <rte_arp.h>
#include <rte_ethdev.h>
#include <rte_icmp.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>

#include "seanet.h"

/*--------------------- debug in/out packet ---------------------*/
void dump_packet(void *arg);
void dump_arp(void *arg);
void dump_ipv4(void *arg);
void dump_ipv6(void *arg);

void dump_eid(void *arg);
void dump_seadp(void *arg);

/*--------------------- debug priority queue ---------------------*/

#endif