#ifndef DM_HEADERS_H
#define DM_HEADERS_H

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

int ip_version(struct ip_header *iph, const unsigned char version)
{
    if (!iph || version > 15)
    {
        return -1;
    }
    iph->_ver_hdrlen &= 0x0F;
    iph->_ver_hdrlen |= version << 4;
    return 0;
}

int ip_hdrlen(struct ip_header *iph, const unsigned char hdrlen)
{
    if (!iph || hdrlen > 15)
    {
        return -1;
    }
    iph->_ver_hdrlen &= 0xF0;
    iph->_ver_hdrlen |= hdrlen;
    return 0;
}

int ip_flags(struct ip_header *iph, const uint16_t flags)
{
    if (!iph || flags > 7)
    {
        return -1;
    }
    iph->_flags_offset &= 0x1FFF;
    iph->_flags_offset |= flags << 13;
    return 0;
}

int ip_offset(struct ip_header *iph, const uint16_t frag_offset)
{
    if (!iph || frag_offset > 8191)
    {
        return -1;
    }
    iph->_flags_offset &= 0xE000;
    iph->_flags_offset |= frag_offset;
    return 0;
}

int tcp_offset(struct tcp_header *tcph, const uint16_t offset)
{
    if (!tcph || offset > 15)
    {
        return -1;
    }
    tcph->_offset_flags &= 0x0FFF;
    tcph->_offset_flags |= offset << 12;
    return 0;
}

int tcp_flags(struct tcp_header *tcph, const uint16_t flags)
{
    if (!tcph || flags >= 512)
    {
        return -1;
    }
    tcph->_offset_flags &= 0xF000;
    tcph->_offset_flags |= flags;
    return 0;
}

#endif
