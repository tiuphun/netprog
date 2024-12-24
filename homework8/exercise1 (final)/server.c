
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/uio.h>

#define BUFFER_SIZE 1024

void split_string(const char *input, char *alphabets, char *digits, int *undefined_count) {
    *undefined_count = 0;
    while (*input) {
        if (isalpha(*input)) {
            *alphabets++ = *input;
        } else if (isdigit(*input)) {
            *digits++ = *input;
        } else {
            (*undefined_count)++;
        }
        input++;
    }
    *alphabets = '\0';
    *digits = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %s...\n", argv[1]);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) {
                break;
            }

            if (buffer[0] == '\0') {
                break;
            }

            char alphabets[BUFFER_SIZE] = {0};
            char digits[BUFFER_SIZE] = {0};
            int undefined_count;

            split_string(buffer, alphabets, digits, &undefined_count);

            struct iovec iov[3];
            iov[0].iov_base = alphabets;
            iov[0].iov_len = strlen(alphabets) + 1;
            iov[1].iov_base = digits;
            iov[1].iov_len = strlen(digits) + 1;

            char undefined_msg[50];
            if (undefined_count > 0) {
                snprintf(undefined_msg, sizeof(undefined_msg), "There are %d undefined characters\n", undefined_count);
            } else {
                snprintf(undefined_msg, sizeof(undefined_msg), "\0");
            }
            iov[2].iov_base = undefined_msg;
            iov[2].iov_len = strlen(undefined_msg) + 1;

            writev(client_fd, iov, 3);
        }

        printf("Client disconnected.\n");
        close(client_fd);
    }

    close(server_fd);
    return 0;
}