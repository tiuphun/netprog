#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5501
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char file_path[256];
    ssize_t bytes_read, bytes_sent = 0;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);


    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server.\n");

    printf("Enter the path of the text file to send: ");
    scanf("%s", file_path);

    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("File opening failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (write(sock_fd, buffer, bytes_read) < 0) {
            perror("Failed to send file data to server");
            fclose(file);
            close(sock_fd);
            exit(EXIT_FAILURE);
        }
        bytes_sent += bytes_read;
    }
    fclose(file);
    printf("File sent to server. Total bytes sent: %zd\n", bytes_sent);

    FILE *received_file = fopen("received_from_server.txt", "w");
    if (!received_file) {
        perror("File opening failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(sock_fd, buffer, BUFFER_SIZE)) > 0) {
        printf("Received data chunk: %.*s\n", (int)bytes_read, buffer); // Debugging
        fwrite(buffer, 1, bytes_read, received_file);
    }
    fclose(received_file);
    printf("Capitalized file received and saved as 'received_from_server.txt'.\n");

    close(sock_fd);
    printf("Disconnected from server.\n");

    return 0;
}
