#ifndef DM_CLNTSRVR_H
#define DM_CLNTSRVR_H
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DGRAM_LEN   sizeof(struct ip_header) + sizeof(struct udp_header)
#define IP_VERSION  4
#define IP_HDR_LEN  5
/*#define DST_ADDR    "24.87.50.12"*/
/*#define DST_ADDR    "192.168.0.103"*/
#define DST_ADDR    "63.223.113.57"
#define SRC_ADDR    "10.211.55.255"
#define DST_PORT    80
#define FILENAME    "secret.txt"
#define RECV_BUFLEN 8096

void client();
int raw_socket();
void send_encoded(int sd, char *dgram, uint16_t len, struct sockaddr_in *din);

void server();
void rcv_encoded(uint16_t *ids, uint16_t len);
void decode(char *decoded, uint16_t initid, uint16_t *ids, int len);

#endif
