#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 5500
#define BUFFER_SIZE 1024

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        bytes_read = read(client_fd, buffer, BUFFER_SIZE);

        if (bytes_read < 0) {
            perror("Read error");
            break;
        } else if (bytes_read == 0) {
            printf("Client disconnected\n");
            break;
        }

        printf("Received message: %s\n", buffer);

        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0) {
            printf("Client sent quit. Closing...\n");
            break;
        }

        for (int i = 0; i < bytes_read; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        if (write(client_fd, buffer, bytes_read) < 0) {
            perror("Write error");
            break;
        }
    }
    close(client_fd);
    exit(0);
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Binding successful\n");

    if (listen(server_fd, 5) < 0) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on %s:%d\n", "127.0.0.1", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected\n");

        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(client_fd);
            continue;
        }
        if (pid == 0) {
            close(server_fd);
            handle_client(client_fd);
        } else {
            close(client_fd); // file descriptor now managed by child proc
        }
    }
    
    close(server_fd);
    return 0;
}
