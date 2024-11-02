#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5501
#define BUFFER_SIZE 1024
#define END_OF_TRANSMISSION "END_OF_FILE"

void capitalize_text(char *text) {
    for (int i = 0; text[i]; i++) {
        text[i] = toupper((unsigned char)text[i]);
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on %s:%d\n", SERVER_IP, PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("Client connection failed");
            continue;
        }
        printf("Client connected.\n");

        FILE *temp_file = fopen("received_file.txt", "w+");
        if (!temp_file) {
            perror("File opening failed");
            close(client_fd);
            continue;
        }

        // Step 1: Receive data from the client
        while ((bytes_read = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
            buffer[bytes_read] = '\0';  // Null-terminate to handle as a string
            if (strcmp(buffer, END_OF_TRANSMISSION) == 0) {
                break;  // End of transmission signal received
            }
            fwrite(buffer, 1, bytes_read, temp_file);
            printf("Received data chunk: %s\n", buffer); // Debugging
        }

        // Step 2: Process the file and capitalize text
        rewind(temp_file);
        FILE *output_file = fopen("output_file.txt", "w");
        if (!output_file) {
            perror("Output file opening failed");
            fclose(temp_file);
            close(client_fd);
            continue;
        }

        while (fgets(buffer, BUFFER_SIZE, temp_file) != NULL) {
            capitalize_text(buffer);
            fputs(buffer, output_file);
        }
        fclose(temp_file);
        fclose(output_file);

        // Step 3: Send the capitalized content back to the client
        output_file = fopen("output_file.txt", "r");
        if (!output_file) {
            perror("Output file opening failed");
            close(client_fd);
            continue;
        }

        while (fgets(buffer, BUFFER_SIZE, output_file) != NULL) {
            write(client_fd, buffer, strlen(buffer));
        }
        fclose(output_file);

        // Send end of transmission signal
        write(client_fd, END_OF_TRANSMISSION, strlen(END_OF_TRANSMISSION));

        close(client_fd);
        printf("Client disconnected.\n");
        printf("Server closing...");
        break;
    }

    close(server_fd);
    return 0;
}
