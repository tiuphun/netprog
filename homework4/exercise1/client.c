#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP_Address> <Port_Number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    char *ip_address = argv[1];
    int port = atoi(argv[2]);

    // Step 1: Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Step 2: Set up the server address struct
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    // Step 3: Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter a string to send to the server (or a blank string to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        // Send the string to the server
        send(sock, buffer, strlen(buffer), 0);

        if (strlen(buffer) == 0) {
            printf("Blank string entered. Exiting.\n");
            break;
        }

        // Receive response from the server
        int valread = read(sock, buffer, BUFFER_SIZE);
        buffer[valread] = '\0'; // Null-terminate the received string
        printf("Received from server:\n%s\n", buffer);
    }

    close(sock);
    return 0;
}
