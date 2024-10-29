#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        return 1;
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int port = atoi(argv[1]);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    while (1)
    {
        int addrlen = sizeof(address);
        printf("Waiting for a new connection...\n");

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }
        printf("Connection accepted\n");

        int valread = read(new_socket, buffer, BUFFER_SIZE);
        printf("Received: %s\n", buffer);

        send(new_socket, buffer, strlen(buffer), 0);
        printf("Response sent\n");

        close(new_socket);
    }
    
    close(server_fd);
    return 0;
}