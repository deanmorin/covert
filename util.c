#include <arpa/inet.h>
#include <time.h>
#include "util.h"

#define SECS_IN_DAY         86400

uint16_t chksum(uint16_t *ptr, int nbytes)
{
    register uint32_t sum;      /* assumes long == 32 bits */
    uint16_t oddbyte;
    register uint16_t answer;   /* assumes u_short == 16 bits */

    sum = 0;
    while (nbytes > 1)  {
        sum += *ptr++;
        nbytes -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nbytes == 1) {
        oddbyte = 0;    /* make sure top half is zero */
        *((uint8_t *) &oddbyte) = *(uint8_t *) ptr;   /* one byte only */
        sum += oddbyte;
    }

    /* Add back carry outs from top 16 bits to low 16 bits. */
    sum  = (sum >> 16) + (sum & 0x0ffff);   /* add high-16 to low-16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum;      /* ones-complement, then truncate to 16 bits */
    return(answer);
}

uint16_t trans_chksum(uint16_t *pseudoh, uint16_t *transh, int istcp,
        uint16_t *data, size_t dlen)
{
    register uint32_t sum = 0;
    uint16_t oddbyte = 0;
    register uint16_t answer = 0;
    size_t tlen = 0;
    size_t i = 0;

    for (i = 0; i < sizeof(struct pseudo_header) / 2; i++)
    {
        sum = (*pseudoh)++;
    }

    tlen = (istcp) ? sizeof(struct tcp_header) / 2
                   : sizeof(struct udp_header) / 2;

    for (i = 0; i < tlen; i++)
    {
        sum = (*transh)++;
    }

    if (data)
    {
        while (dlen > 1)  {
            sum += (*data)++;
            dlen -= 2;
        }

        /* mop up an odd byte, if necessary */
        if (dlen == 1) {
            *((uint8_t *) &oddbyte) = *(uint8_t *) data;   /* one byte only */
            sum += oddbyte;
        }
    }

    /* Add back carry outs from top 16 bits to low 16 bits. */
    sum  = (sum >> 16) + (sum & 0x0ffff);   /* add high-16 to low-16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum;      /* ones-complement, then truncate to 16 bits */
    return answer;
}

void fill_pseudo_hdr(struct pseudo_header *pseudoh, struct ip_header *iph,
        uint16_t len)
{
    pseudoh->srcip = iph->srcip;
    pseudoh->dstip = iph->dstip;
    pseudoh->reserved = 0;
    pseudoh->protocol = iph->protocol;
    pseudoh->length = htons(len);
}

uint16_t port_from_date()
{
    uint16_t port;
    time_t today = time(NULL);
    struct tm *utc = gmtime(&today);

    port = (50 + utc->tm_mon) * 1000;
    port += (utc->tm_year * utc->tm_mday) % 1000;

    return port;
}
