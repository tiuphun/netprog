#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server_IP> <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_fd;
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE], result[BUFFER_SIZE];

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server %s:%s\n", argv[1], argv[2]);

    while (1) {
        printf("Enter domain or IP: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "\n")] = 0;
        if (buffer[0] == '\0') {
            break;
        }

        sendto(client_fd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, server_len);
        memset(result, 0, BUFFER_SIZE);
        recvfrom(client_fd, result, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_len);

        printf("Result:\n%s\n", result);
    }

    close(client_fd);
    return 0;
}