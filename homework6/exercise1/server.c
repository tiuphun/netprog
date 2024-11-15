#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 5500
#define BUFFER_SIZE 1024

// Function to capitalize a string
void capitalize(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

// Thread function to handle each client
void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    free(client_socket);

    while (1) {
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread < 0) {
            perror("Read failed");
            break;
        }

        buffer[valread] = '\0'; // Null-terminate the received string

        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0) {
            printf("Client requested to close the connection.\n");
            break;
        }

        capitalize(buffer);
        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            break;
        }
    }

    close(sock);
    printf("Client disconnected.\n");
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Setting up the address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);

    // Binding the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 10) < 0) { // Set backlog to 10
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on 127.0.0.1:%d\n", PORT);

    while (1) {
        int *new_socket = malloc(sizeof(int)); // Allocate memory for client socket
        if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            free(new_socket);
            continue;
        }

        printf("New client connected.\n");

        // Create a thread for each client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, new_socket) != 0) {
            perror("Thread creation failed");
            free(new_socket);
            continue;
        }

        // Detach the thread so resources are freed after it terminates
        pthread_detach(thread_id);
    }

    close(server_fd);
    return 0;
}
