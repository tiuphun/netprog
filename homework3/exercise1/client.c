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
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    // Convert addr -> binary
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send & receive messages
    while (1) {
        printf("Enter a string: ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input)-1] = '\0'; // remove newline char
        if (strlen(input) == 0 || strcmp(input, "***") == 0) {
            break;
        }
        sendto(sockfd, input, strlen(input), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

        int n = recvfrom(sockfd, response, MAXBUFLEN, 0, NULL, NULL);
        response[n] = '\0';  // Null-terminate the received string

        // Check if it's an error message
        if (strncmp(response, "Error", 5) == 0) {
            // If error, print it and skip receiving the second response
            printf("%s\n", response);
            continue;  // Go back to get a new input
        }

        // If it's not an error, print the alphabetic part
        printf("%s\n", response);

        n = recvfrom(sockfd, response, MAXBUFLEN, 0, NULL, NULL);
        response[n] = '\0';  // Null-terminate the received string
        printf("%s\n", response);
        
    }

    close(sockfd);
    return 0;
}
