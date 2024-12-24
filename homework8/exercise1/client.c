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
        exit(1);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    int client_fd;
    struct sockaddr_in server_addr;

    // Create client socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Connect to server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server %s:%d\n", server_ip, port);

    // char input[BUFFER_SIZE], digits[BUFFER_SIZE], alphabets[BUFFER_SIZE], undefined_msg[BUFFER_SIZE];
    
    

    while (1) {
        // memset(input, 0, BUFFER_SIZE);
        // memset(alphabets, 0, BUFFER_SIZE);
        // memset(digits, 0, BUFFER_SIZE);
        // memset(undefined_msg, 0, BUFFER_SIZE);
        char input[BUFFER_SIZE] = {0};
        char digits[BUFFER_SIZE] = {0};
        char alphabets[BUFFER_SIZE] = {0};
        char undefined_msg[BUFFER_SIZE] = {0};

        printf("Enter a string (blank to exit): ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "") == 0) break;

        // Send string to server
        send(client_fd, input, strlen(input) + 1, 0);

        // Receive results
        struct iovec iov[3];
        iov[0].iov_base = digits;
        iov[0].iov_len = BUFFER_SIZE;
        iov[1].iov_base = alphabets;
        iov[1].iov_len = BUFFER_SIZE;
        iov[2].iov_base = undefined_msg;
        iov[2].iov_len = BUFFER_SIZE;

        int n = readv(client_fd, iov, 3);
        if (n < 0) {
            perror("Error reading from server");
            continue;
        }
//         digits[BUFFER_SIZE - 1] = '\0';
// alphabets[BUFFER_SIZE - 1] = '\0';
// undefined_msg[BUFFER_SIZE - 1] = '\0';

        printf("Digits: %s\n", digits);
        printf("Alphabets: %s\n", alphabets);
        printf("%s\n", undefined_msg);
    }

    close(client_fd);
    return 0;
}
