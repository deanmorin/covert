#include <stdio.h>
#include <stdlib.h>
#include "clntsrvr.h"

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

    if (argc > 1 && strcmp(argv[1], "-s") == 0)
    {
        printf("Starting server...\n");
        server();
    }
    client();

    return 0;
}
