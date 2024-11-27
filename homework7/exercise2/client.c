#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void error_handling(char *message);
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUFFER_SIZE];
    int str_len;

    if (argc != 3) {
        printf("Usage: %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    while (1) {
        printf("Enter UserID: ");
        fgets(message, BUFFER_SIZE, stdin);
        trim_newline(message);
        // printf("%s", message);
        write(sock, message, strlen(message));

        printf("Enter Password: ");
        fgets(message, BUFFER_SIZE, stdin);
        trim_newline(message);
        // printf("%s", message);
        write(sock, message, strlen(message));

        str_len = read(sock, message, BUFFER_SIZE - 1);
        if (str_len == -1)
            error_handling("read() error");

        message[str_len] = 0;
        printf("Message from server: %s\n", message);

        // if (strcmp(message, "Login successful\n") == 0) {
        //     break; // Exit after successful login
        // } else if (strcmp(message, "Account locked\n") == 0) {
        //     close(sock);
        //     exit(1);
        // }
        break;
    }

    close(sock);
    return 0;
}

void error_handling(char *message) {
    perror(message);
    exit(1);
}
