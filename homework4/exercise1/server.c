#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int port = atoi(argv[1]);
    char buffer[BUFFER_SIZE];

    // Step 1: Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Bind the socket to the port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Step 3: Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", port);

    while (1) {
        int addrlen = sizeof(address);
        printf("Waiting for a connection...\n");

        // Step 4: Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        printf("Connection accepted\n");

        // Step 5: Receive and process data from the client
        int valread;
        while ((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
            buffer[valread] = '\0'; // Null-terminate the received string
            printf("Received: %s\n", buffer);

            if (strlen(buffer) == 0) {
                // Exit loop if the client sends a blank string
                printf("Blank string received. Ending session.\n");
                break;
            }

            char alphabets[BUFFER_SIZE] = {0};
            char digits[BUFFER_SIZE] = {0};
            int has_non_alphanumeric = 0;

            // Process the received string
            for (int i = 0; i < strlen(buffer); i++) {
                if (isalpha(buffer[i])) {
                    strncat(alphabets, &buffer[i], 1);
                } else if (isdigit(buffer[i])) {
                    strncat(digits, &buffer[i], 1);
                } else {
                    has_non_alphanumeric = 1;
                    break;
                }
            }

            char response[BUFFER_SIZE] = {0};
            if (has_non_alphanumeric) {
                strcpy(response, "Error: Input contains non-alphanumeric characters.\n");
            } else {
                snprintf(response, BUFFER_SIZE, "Alphabets: %s\nDigits: %s\n", alphabets, digits);
            }

            // Send the result or error message back to the client
            send(new_socket, response, strlen(response), 0);
            printf("Response sent to client.\n");
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}
