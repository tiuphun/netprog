#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5500
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_sent = 0;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return 1;
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);

    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        // Send message to server
        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            return 1;
        }
        bytes_sent += strlen(buffer);

        // Check for termination condition
        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0) {
            printf("Terminating connection...\n");
            break;
        }

        // Receive response from server
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            perror("Receive failed");
            return 1;
        }
        buffer[bytes_received] = '\0';
        printf("Server response: %s\n", buffer);
    }

    // Close socket
    close(sock);
    printf("Total bytes sent: %d\n", bytes_sent);

    return 0;
}