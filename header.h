#ifndef DM_HEADER_H
#define DM_HEADER_H

#include <stdint.h>

#define TCP_FIN     0x0001
#define TCP_SYN     0x0002
#define TCP_RST     0x0004
#define TCP_PSH     0x0008
#define TCP_ACK     0x0010
#define TCP_URG     0x0020
#define TCP_ECE     0x0040
#define TCP_CWR     0x0080
#define TCP_NS      0x0100

struct ip_header
{
    uint8_t     _ver_hdrlen;
    uint8_t     tos;
    int16_t    length; 
    uint16_t    id;
    uint16_t    _flags_offset;
    uint8_t     ttl;
    uint8_t     protocol;
    uint16_t    checksum;
    uint32_t    srcip;
    uint32_t    dstip;
};

struct tcp_header
{
    uint16_t    source_port;
    uint16_t    destPort;
    uint32_t    sequence;
    uint32_t    ack;
    uint16_t    _offset_flags;
    uint16_t    window;
    uint16_t    checksum;
    uint16_t    urgent;
};

struct udp_header
{
    uint16_t    srcport;
    uint16_t    dstport;
    uint16_t    length;
    uint16_t    checksum;
};

struct pseudo_header
{
    uint32_t    srcip;
    uint32_t    dstip;
    uint8_t     reserved;
    uint8_t     protocol;
    uint16_t    length;
};

int ip_version(struct ip_header *iph, const unsigned char version);
int ip_hdrlen(struct ip_header *iph, const unsigned char hdrlen);
int ip_flags(struct ip_header *iph, const uint16_t flags);
int ip_offset(struct ip_header *iph, const uint16_t frag_offset);
int tcp_offset(struct tcp_header *tcph, const uint16_t offset);
int tcp_flags(struct tcp_header *tcph, const uint16_t flags);

#endif
