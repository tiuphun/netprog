#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXBUFLEN 1024

char input[MAXBUFLEN], response[MAXBUFLEN];
int sockfd;
struct sockaddr_in server_addr;
socklen_t server_addr_len = sizeof(server_addr);

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IPAdress> <PortNumber>\n", argv[0]);
        return 1;
    }

    char* server_ip = argv[1];
    int port = atoi(argv[2]);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)