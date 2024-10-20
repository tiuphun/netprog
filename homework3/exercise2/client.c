#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s IPAddress PortNumber\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter an IP address or domain name (or blank to exit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline

        if (strlen(buffer) == 0) {
            break;
        }

        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));

        char response[BUFFER_SIZE];
        socklen_t addr_len = sizeof(server_addr);
        ssize_t recv_len = recvfrom(sockfd, response, BUFFER_SIZE, 0, (struct sockaddr *) &server_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom failed");
            continue;
        }
        response[recv_len] = '\0'; // Null-terminate the received string

        printf("%s\n", response);
    }

    close(sockfd);
    return 0;
}
