#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    int client_fd;
    struct sockaddr_in server_addr;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }  

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
    {
        perror("invalid address");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    char file_path[BUFFER_SIZE];
    printf("Enter file path (or Enter to exit): ");
    fgets(file_path, BUFFER_SIZE, stdin);
    file_path[strcspn(file_path, "\n")] = '\0'; // Remove trailing newline

    if (strlen(file_path) == 0)
    {
        printf("No file path entered. Exiting...\n");
        close(client_fd);
        exit(EXIT_SUCCESS);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        perror("file open failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    //check file size
    struct stat file_stat;
    if (stat(file_path, &file_stat) == -1)
    {
        perror("Cannot get file size");
        close(client_fd);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (file_stat.st_size > 100 * 1024 * 1024)
    {
        printf("Error: File size exceeds 100MB\n");
        close(client_fd);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // After getting the file path, before the file sending logic
    char *filename = strrchr(file_path, '/');
    if (filename != NULL) {
        filename++; // Move past the last '/'
    } else {
        filename = file_path; // No '/' found, use the full path
    }


    send(client_fd, filename, strlen(filename) + 1, 0); // +1 to send the null terminator

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_sent;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if ((bytes_sent = send(client_fd, buffer, bytes_read, 0)) == -1)
        {
            perror("send failed");
            close(client_fd);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }
    printf("File sent successfully\n");

    fclose(file);
    
    close(client_fd);
    return 0;
}
