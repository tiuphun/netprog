
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_Address> <Port_Number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    while (1) {
        printf("Enter a string: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remove newline character
        buffer[strcspn(buffer, "\n")] = 0;

        if (buffer[0] == '\0') {
            break;
        }

        send(client_fd, buffer, strlen(buffer), 0);

        char alphabets[BUFFER_SIZE] = {0};
        char digits[BUFFER_SIZE] = {0};
        char undefined_msg[BUFFER_SIZE] = {0};

        struct iovec iov[3];
        iov[0].iov_base = alphabets;
        iov[0].iov_len = BUFFER_SIZE;
        iov[1].iov_base = digits;
        iov[1].iov_len = BUFFER_SIZE;
        iov[2].iov_base = undefined_msg;
        iov[2].iov_len = BUFFER_SIZE;

        readv(client_fd, iov, 3);

        printf("Alphabets: %s\n", alphabets);
        printf("Digits: %s\n", digits);
        if (undefined_msg[0] != '\0') {
            printf("%s", undefined_msg);
        }
    }

    close(client_fd);
    return 0;
}