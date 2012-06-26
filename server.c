#include "clntsrvr.h"
#include "header.h"
#include "network.h"
#include "util.h"

void server()
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sd;
    char buf[RECV_BUFLEN];
    uint16_t len;
    uint16_t *ids;
    char *decoded;

    struct ip_header *iph = (struct ip_header*) buf;
    struct udp_header *udph = (struct udp_header *)
            (buf + sizeof(struct ip_header));


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
            printf("Covert message arriving...\n");
            len = iph->id;
            ids = malloc(sizeof(uint16_t) * len * 2);
            decoded = malloc(sizeof(char) * len + 1);

            rcv_encoded(ids, len, sd, buf, &client);
            printf("Message received. Decoding...\n\n");

            decode(decoded, ids, len);
            printf("%s\n", decoded);

            free(ids);
            free(decoded);
        }
#ifdef DEBUG
        else
        {
            /*printf("A message we don't care about...\n");*/
            /*printf("  ntohs(udph->srcport):  %d\n", ntohs(udph->srcport));*/
            /*printf("  port_from_date():      %d\n", port_from_date());*/
            /*printf("  IP_FLAGS(iph):         %d\n", IP_FLAGS(iph));*/
            /*printf("  IP_DONTFRAG:           %d\n", IP_DONTFRAG);*/
        }
#endif
    }
}

void rcv_encoded(uint16_t *ids, uint16_t len, int sd, char *buf,
        struct sockaddr_in *client)
{
    int i;
    int dgram_num;
    uint16_t id;
    struct ip_header *iph = (struct ip_header*) buf;
    struct udp_header *udph = (struct udp_header *)
            (buf + sizeof(struct ip_header));
    uint16_t initid = ntohs(len);


    printf("Message length: %d\n", len);

    for (i = 0; i < len * 2; i++)
    {
        recv_dgram(sd, buf, client);

        id = ntohs(iph->id);

        if (ntohs(udph->srcport) != port_from_date()
                || IP_FLAGS(iph) == IP_DONTFRAG
                || !in_range(initid, id))
        {
            /* not one of our packets */
#ifdef DEBUG
            /*printf("A message we don't care about...\n");*/
            /*printf("  ntohs(udph->srcport):  %d\n", ntohs(udph->srcport));*/
            /*printf("  port_from_date():      %d\n", port_from_date());*/
            /*printf("  IP_FLAGS(iph):         %d\n", IP_FLAGS(iph));*/
            /*printf("  IP_DONTFRAG:           %d\n", IP_DONTFRAG);*/
            /*printf("  id:                    %d\n", id);*/
            /*printf("  in_range(initid, id):  %d\n", in_range(initid, id));*/
#endif
            i--;
            continue;
        }
        /* len is the initial ID */
        dgram_num = (id - initid - 1) / 0x10;
        ids[dgram_num] = id;

        printf("#%d\tID: %d\n", dgram_num + 1, id);
    }
}

void decode(char *decoded, uint16_t *ids, uint16_t len)
{
    char c;
    int i = 0;
    uint16_t initid = ntohs(len);

    while (i < len * 2)
    {
        c = (ids[i] - 0x10 * i - initid - 1) << 4;
        i++;
        c |= ids[i] - 0x10 * i - initid - 1;
        i++;

        decoded[i / 2 - 1] = c;
    }
    decoded[len] = '\0';
}

void recv_dgram(int sd, char *buf, struct sockaddr_in *client)
{
    socklen_t clen = sizeof(client);

    if (recvfrom(sd, buf, RECV_BUFLEN, 0, (struct sockaddr *) client,
            &clen) == -1)
    {
        exit(sock_error("recvfrom()", 0));
    }
}

int in_range(uint16_t initid, uint16_t id)
{
    uint16_t len = htons(initid);

    return (id > initid && id <= initid + len * 0x10 * 2);
}
