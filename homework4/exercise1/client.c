#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IPAdress> <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    char *ip_address = argv[1];
    int port = atoi(argv[2]);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter a string: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;  // Remove newline character

    send(sock, buffer, strlen(buffer), 0);
    printf("Message sent\n");

    int valread = read(sock, buffer, BUFFER_SIZE);
    printf("Received: %s\n", buffer);

    close(sock);
    return 0;
}