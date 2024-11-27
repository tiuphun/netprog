#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/select.h>

#define BUF_SIZE 1024

void encode_decode_file(FILE *input, FILE *output, int key, int opcode, int *error);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(1);
    }

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    fd_set read_fds, temp_fds;
    int max_fd;
    char buffer[BUF_SIZE];
    int key, opcode;
    unsigned short length;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    // Bind server socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("listen");
        close(server_socket);
        exit(1);
    }

    printf("Server is listening on port %s...\n", argv[1]);

    // Initialize fd sets
    FD_ZERO(&read_fds);
    FD_SET(server_socket, &read_fds);
    max_fd = server_socket;

    while (1) {
        temp_fds = read_fds; // Copy the set

        // Use select to monitor sockets
        if (select(max_fd + 1, &temp_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &temp_fds)) {
                if (i == server_socket) {
                    // New connection
                    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
                    if (client_socket < 0) {
                        perror("accept");
                        continue;
                    }
                    printf("New client connected: %d\n", client_socket);
                    FD_SET(client_socket, &read_fds);
                    if (client_socket > max_fd) {
                        max_fd = client_socket;
                    }
                } else {
                    // Data from a client
                    int client = i;
                    if (recv(client, &opcode, sizeof(opcode), 0) <= 0 ||
                        recv(client, &length, sizeof(length), 0) <= 0 ||
                        recv(client, &key, sizeof(key), 0) <= 0) {
                        printf("Client %d disconnected.\n", client);
                        close(client);
                        FD_CLR(client, &read_fds);
                        continue;
                    }

                    printf("Received opcode: %d, key: %d, length: %d\n", opcode, key, length);

                    // Receive file
                    FILE *temp_file = fopen("temp_file.txt", "wb");
                    if (!temp_file) {
                        perror("fopen");
                        close(client);
                        FD_CLR(client, &read_fds);
                        continue;
                    }

                    long total_received = 0;
                    while (total_received < length) {
                        ssize_t bytes_received = recv(client, buffer, BUF_SIZE, 0);
                        if (bytes_received < 0) {
                            perror("recv");
                            fclose(temp_file);
                            close(client);
                            FD_CLR(client, &read_fds);
                            continue;
                        }
                        fwrite(buffer, 1, bytes_received, temp_file);
                        total_received += bytes_received;
                    }

                    fclose(temp_file);

                    // Process file
                    temp_file = fopen("temp_file.txt", "rb");
                    FILE *output_file = fopen("output_file.txt", "wb");
                    if (!temp_file || !output_file) {
                        perror("fopen");
                        close(client);
                        FD_CLR(client, &read_fds);
                        continue;
                    }

                    int error_flag = 0;
                    encode_decode_file(temp_file, output_file, key, opcode, &error_flag);

                    fclose(temp_file);
                    fclose(output_file);

                    // Send result or error
                    if (error_flag) {
                        opcode = 3; // Error
                        send(client, &opcode, sizeof(opcode), 0);
                    } else {
                        output_file = fopen("output_file.txt", "rb");
                        if (!output_file) {
                            perror("fopen");
                            close(client);
                            FD_CLR(client, &read_fds);
                            continue;
                        }

                        while (1) {
                            ssize_t bytes_read = fread(buffer, 1, BUF_SIZE, output_file);
                            if (bytes_read > 0) {
                                send(client, buffer, bytes_read, 0);
                            } else if (feof(output_file)) {
                                break;
                            } else if (ferror(output_file)) {
                                perror("fread");
                                break;
                            }
                        }

                        fclose(output_file);
                    }

                    // Clean up
                    remove("temp_file.txt");
                    remove("output_file.txt");
                    close(client);
                    FD_CLR(client, &read_fds);
                }
            }
        }
    }

    close(server_socket);
    return 0;
}

void encode_decode_file(FILE *input, FILE *output, int key, int opcode, int *error) {
    char ch;
    while ((ch = fgetc(input)) != EOF) {
        if (opcode == 0) { // Encode
            if (isalpha(ch)) {
                if (isupper(ch)) {
                    ch = (ch - 'A' + key) % 26 + 'A';
                } else {
                    ch = (ch - 'a' + key) % 26 + 'a';
                }
            }
        } else if (opcode == 1) { // Decode
            if (isalpha(ch)) {
                if (isupper(ch)) {
                    ch = (ch - 'A' - key + 26) % 26 + 'A';
                } else {
                    ch = (ch - 'a' - key + 26) % 26 + 'a';
                }
            }
        } else {
            *error = 1;
            printf("Invalid opcode.\n");
            return;
        }

        if (fputc(ch, output) == EOF) {
            *error = 1;
            printf("Error writing to file.\n");
            return;
        }
    }
}
