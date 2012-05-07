#include "header.h"

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
