#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

void handle_request(int sockfd, struct sockaddr_in *client_addr) {
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(*client_addr);
    
    ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) client_addr, &addr_len);
    if (recv_len < 0) {
        perror("recvfrom failed");
        return;
    }
    buffer[recv_len] = '\0';  

    char response[BUFFER_SIZE];
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_DGRAM;

    struct in_addr addr;

    if (inet_pton(AF_INET, buffer, &addr) == 1) {
        struct sockaddr_in sa;
        char host[BUFFER_SIZE];
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_addr = addr;

        if (getnameinfo((struct sockaddr *)&sa, sizeof(sa), host, sizeof(host), NULL, 0, 0) == 0) {
            snprintf(response, BUFFER_SIZE, "Official name: %s", host);
        } else {
            snprintf(response, BUFFER_SIZE, "Not found information");
        }
    } else {
        if (getaddrinfo(buffer, NULL, &hints, &result) == 0) {
            snprintf(response, BUFFER_SIZE, "Official IP: ");
            char ip[INET_ADDRSTRLEN];
            int first_alias = 1; // To manage the format for alias IPs

            for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
                if (p->ai_family == AF_INET) { // IPv4
                    struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
                    inet_ntop(AF_INET, &(ipv4->sin_addr), ip, sizeof(ip));

                    if (first_alias) {
                        strcat(response, ip); // Add the first IP as the official
                        strcat(response, "\nAlias IP: ");
                        first_alias = 0; // Set to false after the first IP
                    } else {
                        strcat(response, ip); // Add alias IP
                        strcat(response, "\n"); // Separate IPs with newline
                    }
                }
            }

            if (first_alias) {
                snprintf(response, BUFFER_SIZE, "Not found information");
            } else {
                response[strlen(response) - 1] = '\0'; // Remove last newline
                strcat(response, "\n"); // Add newline at the end
            }

            freeaddrinfo(result);
        } else {
            snprintf(response, BUFFER_SIZE, "Invalid IP address");
        }
    }

    sendto(sockfd, response, strlen(response), 0, (struct sockaddr *) client_addr, addr_len);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s PortNumber\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %s...\n", argv[1]);

    while (1) {
        handle_request(sockfd, &client_addr);
    }

    close(sockfd);
    return 0;
}
