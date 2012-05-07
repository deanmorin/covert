#include "clntsrvr.h"
#include "header.h"
#include "network.h"
#include "util.h"

void server()
{
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sd;

    if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        exit(sock_error("socket()", 0));
    }

    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    
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
