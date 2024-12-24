#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUF_SIZE 1024

void send_file(FILE *file, int socket);
void receive_file(int socket, const char *output_filename);

int main(int argc, char *argv[]) {
    char server_ip[INET_ADDRSTRLEN];
    int server_port;
    int opcode, key;
    char input_filename[256];
    char output_filename[256];

    // Check if IP and port are provided as arguments
    if (argc != 3) {
        printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    // Get IP and port from arguments
    strncpy(server_ip, argv[1], sizeof(server_ip));
    server_port = atoi(argv[2]);

    // Validate port number
    if (server_port <= 0 || server_port > 65535) {
        printf("Invalid port number. Please enter a valid port between 1 and 65535.\n");
        exit(1);
    }

    printf("Connected to server: %s:%d\n", server_ip, server_port);

    // Prompt for operation code, key, input file, and output file
    printf("Choose operation (0 = encode, 1 = decode): ");
    scanf("%d", &opcode);
    while (opcode != 0 && opcode != 1) {
        printf("Invalid opcode. Please choose 0 (encode) or 1 (decode): ");
        scanf("%d", &opcode);
    }

    printf("Enter Caesar cipher key: ");
    scanf("%d", &key);

    printf("Enter input file name: ");
    scanf("%s", input_filename);

    printf("Enter output file name: ");
    scanf("%s", output_filename);

    int client_socket;
    struct sockaddr_in server_addr;
    FILE *input_file;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(client_socket);
        exit(1);
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(client_socket);
        exit(1);
    }

    // Open input file
    input_file = fopen(input_filename, "rb");
    if (!input_file) {
        perror("fopen");
        close(client_socket);
        exit(1);
    }

    // Get file size
    fseek(input_file, 0, SEEK_END);
    unsigned short length = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    printf("File size: %d bytes\n", length);

    // Send opcode, length, and key
    if (send(client_socket, &opcode, sizeof(opcode), 0) < 0 ||
        send(client_socket, &length, sizeof(length), 0) < 0 ||
        send(client_socket, &key, sizeof(key), 0) < 0) {
        perror("send");
        fclose(input_file);
        close(client_socket);
        exit(1);
    }

    // Send file
    printf("Sending file to server...\n");
    send_file(input_file, client_socket);
    fclose(input_file);

    // Receive result file
    printf("Receiving processed file from server...\n");
    receive_file(client_socket, output_filename);

    printf("File saved as: %s\n", output_filename);

    close(client_socket);
    return 0;
}

void send_file(FILE *file, int socket) {
    char buffer[BUF_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUF_SIZE, file)) > 0) {
        if (send(socket, buffer, bytes_read, 0) < 0) {
            perror("send");
            exit(1);
        }
    }
}

void receive_file(int socket, const char *output_filename) {
    FILE *output_file = fopen(output_filename, "wb");
    if (!output_file) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = recv(socket, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, output_file);
    }

    if (bytes_received < 0) {
        perror("recv");
    }

    fclose(output_file);
}
