#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "header.h"
#include "network.h"

#define DGRAM_LEN   sizeof(struct ip_header) + sizeof(struct udp_header)
#define IP_VERSION  4
#define IP_HDR_LEN  5
#define SRC_ADDR    "24.87.50.12"
/*#define DST_ADDR    "63.223.113.57"*/
#define DST_ADDR    "10.211.55.255"
#define SRC_PORT    33333
#define DST_PORT    80

/* 
 * Copyright (c)1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * dupliated in all such forms and that any documentation, advertising 
 * materials, and other materials related to such distribution and use
 * acknowledge that the software was developed by the University of
 * California, Berkeley. The name of the University may not be used
 * to endorse or promote products derived from this software without
 * specific prior written permission. THIS SOFTWARE IS PROVIDED ``AS
 * IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHATIBILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE
 *
 * Our algorithm is simple, using a 32-bit accumulator (sum),
 * we add sequential 16-bit words to it, and at the end, fold back
 * all the carry bits from the top 16 bits into the lower 16 bits.
 */
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
    pseudoh->length = len;
}

void send_data()
{
    char dgram[DGRAM_LEN];
    int sd;

    int on = 1;
    int i = 0;

    struct ip_header *iph  = (struct ip_header*) dgram;
    struct udp_header *udph = (struct udp_header *)
            (dgram + sizeof(struct ip_header));
    struct pseudo_header pseudoh;
    
    struct sockaddr_in sin;
    struct sockaddr_in din;

    memset(dgram, 0, DGRAM_LEN);
    memset((char *) &sin, 0, sizeof(struct sockaddr_in));
    memset((char *) &din, 0, sizeof(struct sockaddr_in));

    sin.sin_family = AF_INET;
    din.sin_family = AF_INET;
    sin.sin_port = htons(32000);
    din.sin_port = htons(32000);
    sin.sin_addr.s_addr = inet_addr(SRC_ADDR);
    din.sin_addr.s_addr = inet_addr(DST_ADDR);

    ip_version(iph, IP_VERSION);
    ip_hdrlen(iph, IP_HDR_LEN);
    iph->tos = 0;
    iph->length = DGRAM_LEN;
    iph->id = htons(54323); // CHANGE TO MEANINGFUL STARTING NUM
    ip_flags(iph, 0);
    ip_offset(iph, 0);
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->srcip = inet_addr(SRC_ADDR);
    iph->dstip = inet_addr(DST_ADDR);

    udph->srcport = htons(SRC_PORT);
    udph->dstport = htons(DST_PORT);
    udph->length = htons(sizeof(struct udp_header));
    udph->checksum = trans_chksum((uint16_t *) udph, (uint16_t *) &pseudoh,
            0, NULL, 0);

    fill_pseudo_hdr(&pseudoh, iph, sizeof(struct udp_header)); /* + data length */

    iph->checksum = chksum((uint16_t *) dgram, IP_HDR_LEN);

    /*Send loop, send for every 2 second for 100 count*/
    printf("Trying...\n");
    printf("Using raw socket and UDP protocol\n");
    /*printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));*/
    if ((sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) < 0)
    {
        int err = sock_error("socket()", 0);
        fprintf(stderr, "> You need root privileges to run this program.\n");
        exit(err);
    }
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(int)) < 0)
    {
        exit(sock_error("setsockopt()", 0));
    }


 
    for (i = 1; i <= 20; i++)
    {
        if (sendto(sd, dgram, 28, 0, (struct sockaddr *) &din, sizeof(din)) < 0)
        {
            perror("SENDTO");
            exit(sock_error("sendto()", 0));
        }
        else
        {
            printf("Count #%u - sendto() is OK.\n", i);
            sleep(2);
        }
    }
    close(sd);
}

int main(int argc, char **argv)
{
    /*if (setuid(0) || setgid(0))*/
    /*{*/
        /*fprintf(stderr, "> This needs to be owned by root with the s-bit set\n");*/
        /*fprintf(stderr, "chown root %s\n", argv[0]);*/
        /*fprintf(stderr, "chmod +s %s\n", argv[0]);*/
    /*}*/
    /*printf("%d\n", geteuid());*/
    /*printf("%d\n", getegid());*/

    send_data();

    return 0;
}
