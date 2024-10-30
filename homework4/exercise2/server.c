#define _POSIX_C_SOURCE 200112L 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define BACKLOG 5
#define STORAGE_DIR "uploads"
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int port = atoi(argv[1]);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", port);

    if (mkdir(STORAGE_DIR, 0777) == -1 && errno != EEXIST)
    {
        perror("Directory creation failed or already exists");
    } else {
        printf("Created directory %s\n", STORAGE_DIR);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) != -1) {
        printf("Connected to client\n");

        char buffer[BUFFER_SIZE];
        ssize_t bytes_received;

        char filename[BUFFER_SIZE];
        if (recv(new_socket, filename, BUFFER_SIZE, 0) <= 0)
        {
            perror("filename receive failed");
            close(new_socket);
            continue;
        }

        char filepath[BUFFER_SIZE];
        snprintf(filepath, BUFFER_SIZE, "%s/%s", STORAGE_DIR, filename);
        FILE *file = fopen(filepath, "wb");
        if (file == NULL)
        {
            perror("file open failed");
            send(new_socket, "Error: File existed on server", 32, 0);
            close(new_socket);
            continue;
        }

        while((bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0)
        {
            fwrite(buffer, 1, bytes_received, file);
        }

        if (bytes_received < 0) {
            perror("error receiving file");
            send(new_socket, "Error: File transfer failed", 30, 0);
        } else {
            printf("File %s received successfully\n", filename);
            send(new_socket, "Transfer complete", 17, 0);
        }

        fclose(file);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
