#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/uio.h>


#define BUFFER_SIZE 1024

void split_string(const char *input, char *alphabets, char *digits, int *undefined_count) {
    *undefined_count = 0;
    int alpha_index = 0, digit_index = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        if (isalpha(input[i])) {
            alphabets[alpha_index++] = input[i];
        } else if (isdigit(input[i])) {
            digits[digit_index++] = input[i];
        } else {
            (*undefined_count)++;
        }
    }

    alphabets[alpha_index] = '\0';
    digits[digit_index] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set the socket to non-blocking mode
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server is running on port %d\n", port);

    // char alphabets[BUFFER_SIZE], digits[BUFFER_SIZE], response[BUFFER_SIZE];
    // int undefined_count;

    while (1) {
        // Accept client connections
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd >= 0) {
            printf("Client connected\n");

            while (1) {
                memset(buffer, 0, BUFFER_SIZE);
            char alphabets[BUFFER_SIZE] = {0};
            char digits[BUFFER_SIZE] = {0};
            char undefined_msg[BUFFER_SIZE] = {0};
            int undefined_count = 0;
                // memset(buffer, 0, BUFFER_SIZE);
                // memset(alphabets, 0, BUFFER_SIZE);
                // memset(digits, 0, BUFFER_SIZE);
                // memset(response, 0, BUFFER_SIZE);

                int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);

                if (bytes_received > 0) {
                    printf("Received: %s\n", buffer);

                    if (strcmp(buffer, "") == 0) break;

                    split_string(buffer, alphabets, digits, &undefined_count);

                    snprintf(undefined_msg, sizeof(undefined_msg), "Undefined characters: %d", undefined_count);

                    // Send results
                    struct iovec iov[3];
                    iov[0].iov_base = digits;
                    iov[0].iov_len = strlen(digits) + 1;
                    iov[1].iov_base = alphabets;
                    iov[1].iov_len = strlen(alphabets) + 1;

                    // char undefined_msg[50];
                    // snprintf(undefined_msg, sizeof(undefined_msg), "There is %d undefined character(s)", undefined_count);
                    iov[2].iov_base = undefined_msg;
                    iov[2].iov_len = strlen(undefined_msg) + 1;

                    printf("Digits: %s\n", digits);
                    printf("Alphabets: %s\n", alphabets);
                    printf("Undefined characters: %d\n", undefined_count);


                    writev(client_fd, iov, 3);
                   
                } else if (bytes_received == 0) {
                    printf("Client disconnected.\n");
                    break;
                }
            }
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
