#ifndef _SEANET_H_
#define _SEANET_H_

#define IPV6_HDR_LEN 40
#define EID_HDR_LEN 44
#define SEADP_HDR_LEN 20

struct eid_hdr {
    uint8_t next_hdr;
    uint8_t hdr_len;
    uint16_t prop;
    char src_eid[20];
    char dst_eid[20];
} __attribute__((__packed__));

struct seadp_hdr {
    uint8_t type : 4, proto_flag : 4;
    uint8_t hdr_len;
    uint8_t cache_flag;
    uint8_t storage_flag;
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t pkt_no;
    uint16_t checksum;
    uint32_t offset;
    uint32_t chunck_len;
    uint32_t fnumber;
} __attribute__((__packed__));

#endif