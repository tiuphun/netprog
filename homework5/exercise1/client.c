#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5500
#define BUFFER_SIZE 1024

int main()
{
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_sent = 0;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to the server failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    while (1)
    {
        printf("Enter message (q/Q to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        int len = strlen(buffer);
        if (write(client_fd, buffer, len) < 0) {
            perror("Failed to send message");
            break;
        }
        bytes_sent += len;

        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0) {
            printf("Exiting...\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        if (read(client_fd, buffer, BUFFER_SIZE - 1) < 0) {
            perror("Failed to receive message");
            break;
        }
        printf("Server response: %s\n", buffer);

        printf("Total bytes sent to server: %d\n", bytes_sent);
    }
    
    close(client_fd);
    return 0;
}
