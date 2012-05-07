#include "clntsrvr.h"
#include "header.h"
#include "network.h"
#include "util.h"

void recv_dgram(int sd, char *buf, struct sockaddr_in *client)
{
    socklen_t clen = sizeof(client);

    if (recvfrom(sd, buf, RECV_BUFLEN, 0, (struct sockaddr *) client,
            &clen) == -1)
    {
        exit(sock_error("recvfrom()", 0));
    }
}

void server()
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sd;
    char buf[RECV_BUFLEN];

    struct ip_header *iph = (struct ip_header*) buf;
    struct udp_header *udph = (struct udp_header *)
            (buf + sizeof(struct ip_header));


    /* IF PROBLEM, CHANGE LAST ARG TO 0 */
    if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
    {
        exit(sock_error("socket()", 0));
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(DST_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sd, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        exit(sock_error("bind()", 0));
    }

    while (1)
    {
        recv_dgram(sd, buf, &client);

        if (ntohs(udph->srcport) == port_from_date()
                && IP_FLAGS(iph) == IP_DONTFRAG)
        {
            printf("Covert Message Arriving...\n");
        }
        else
        {
            printf("A message we don't care about...\n");
        }
    }
}

void rcv_encoded(uint16_t *ids, uint16_t len)
{
    int i = 0;

    for (i = 0; i < len; i++)
    {
        /*recvfrom(sd);*/
    }
}

void decode(char *decoded, uint16_t initid, uint16_t *ids, int len)
{
    char c;
    int i = 0;

    if (len % 2)
    {
        fprintf(stderr, 
                "There should be an even number of encoded packets received");
        exit(1);
    }

    while (i < len)
    {
        c = (ids[i] - 0x10 * i - initid - 1) << 4;
        i++;
        c = (ids[i] - 0x10 * i - initid - 1) & 0xF0;
        i++;

        decoded[i/2] = c;  
    }
}
