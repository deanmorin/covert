#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "network.h"
#include "util.h"

#define DGRAM_LEN   sizeof(struct ip_header) + sizeof(struct udp_header)
#define IP_VERSION  4
#define IP_HDR_LEN  5
#define DST_ADDR    "24.87.50.12"
/*#define DST_ADDR    "63.223.113.57"*/
#define SRC_ADDR    "10.211.55.255"
#define SRC_PORT    33333
#define DST_PORT    80


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
    iph->length = htons(DGRAM_LEN);
    iph->id = htons(54323); /* CHANGE TO MEANINGFUL STARTING NUM */
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
