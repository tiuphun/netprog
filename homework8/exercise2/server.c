#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

void handle_query(const char *query, char *result) {
    struct addrinfo hints, *res, *p;
    char host[BUFFER_SIZE];
    int found = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(query, NULL, &hints, &res) == 0) {
        for (p = res; p != NULL; p = p->ai_next) {
            if (p->ai_family == AF_INET || p->ai_family == AF_INET6) {
                getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
                strcat(result, host);
                strcat(result, "\n");
                found = 1;
            }
        }
        freeaddrinfo(res);
    } else {
        struct sockaddr_in sa;
        struct sockaddr_in6 sa6;

        if (inet_pton(AF_INET, query, &sa.sin_addr) == 1 || inet_pton(AF_INET6, query, &sa6.sin6_addr) == 1) {
            hints.ai_flags = AI_CANONNAME;
            if (getaddrinfo(query, NULL, &hints, &res) == 0) {
                for (p = res; p != NULL; p = p->ai_next) {
                    if (p->ai_canonname) {
                        strcat(result, p->ai_canonname);
                        strcat(result, "\n");
                        found = 1;
                    }
                }
                freeaddrinfo(res);
            }
        }
    }

    if (!found) {
        strcat(result, "Not found information\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PortNumber>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE], result[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("Server is running on port %s...\n", argv[1]);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        memset(result, 0, BUFFER_SIZE);

        int bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            if (buffer[0] == '\0') {
                break;
            }
            handle_query(buffer, result);
            sendto(server_fd, result, strlen(result), 0, (struct sockaddr *)&client_addr, client_len);
        }
    }

    close(server_fd);
    return 0;
}