#include "clntsrvr.h"
#include "header.h"
#include "network.h"
#include "util.h"

void client()
{
    char dgram[DGRAM_LEN];
    int sd;

    FILE *in = NULL;
    int count = 0;
    char c;

    struct ip_header *iph  = (struct ip_header*) dgram;
    struct udp_header *udph = (struct udp_header *)
            (dgram + sizeof(struct ip_header));
    struct pseudo_header pseudoh;
    
    struct sockaddr_in sin;
    struct sockaddr_in din;

    uint16_t initid;

    if (!(in = fopen(FILENAME, "rb")))
    {
        fprintf(stderr, "Can't open the file \"%s\"\n", FILENAME);
        exit(1);
    }

    fseek(in, 0, SEEK_END);
    /* byte order changed to make encoding calculations easier */
    initid = htons(ftell(in));
    fseek(in, 0, SEEK_SET);

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
    iph->id = htons(initid);   
    ip_flags(iph, 0);
    ip_offset(iph, 0);
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->srcip = inet_addr(SRC_ADDR);
    iph->dstip = inet_addr(DST_ADDR);

    udph->srcport = htons(port_from_date());
    udph->dstport = htons(DST_PORT);
    udph->length = htons(sizeof(struct udp_header));
    /*udph->checksum = trans_chksum((uint16_t *) udph, (uint16_t *) &pseudoh,*/
            /*0, NULL, 0);*/

    fill_pseudo_hdr(&pseudoh, iph, sizeof(struct udp_header)); /* + data length */

    iph->checksum = chksum((uint16_t *) dgram, IP_HDR_LEN);


    sd = raw_socket();


    /*Send loop, send for every 2 second for 100 count*/
    printf("Trying...\n");
    printf("Using raw socket and UDP protocol\n");
    /*printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));*/

    /* initial ID that all encoding will be based off of (also length of msg) */
    send_encoded(sd, dgram, ntohs(iph->length), &din);

    while ((c = fgetc(in)) != EOF)
    {
        /* send half a char at a time */
        int fhalf = c >> 4;
        int shalf = c & 0xF;

        iph->id = htons(initid + 0x10 * count++ + fhalf + 1);
        send_encoded(sd, dgram, ntohs(iph->length), &din);
        iph->id = htons(initid + 0x10 * count++ + shalf + 1);
        send_encoded(sd, dgram, ntohs(iph->length), &din);
    }
    close(sd);
}

int raw_socket()
{
    int sd;
    int on = 1;

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
    return sd;
}

void send_encoded(int sd, char *dgram, uint16_t len, struct sockaddr_in *din) 
{
    static int i = 0;

    if (sendto(sd, dgram, len, 0, (struct sockaddr *) din, 
            sizeof(struct sockaddr_in)) < 0)
    {
        perror("SENDTO");
        exit(sock_error("sendto()", 0));
    }
    else
    {
        printf("Count #%u - sendto() is OK.\n", i++);
        usleep(100);
    }
}
