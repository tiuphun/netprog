#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>

#define MAXBUFLEN 1024

char buffer[MAXBUFLEN];
int sockfd;
struct sockaddr_in server_addr, client_addr;
socklen_t client_addr_len = sizeof(client_addr);

void create_socket(int port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // bind socket to given port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d\n", port);
}

void receive_string() {
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (n < 0) {
        perror("recvfrom failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';

    printf("Received string from client: %s\n", buffer);
}

void process_string() {
    char alphabets[MAXBUFLEN], digits[MAXBUFLEN];
    int alpha_index = 0, digit_index = 0;
    int invalid = 0;

    for (int i = 0; buffer[i] != '\0'; i++) {
        if (isalpha(buffer[i])) {
            alphabets[alpha_index++] = buffer[i];
        } else if (isdigit(buffer[i])) {
            digits[digit_index++] = buffer[i];
        } else {
            invalid = 1;
            break;
        }
    }

    alphabets[alpha_index] = '\0';
    digits[digit_index] = '\0';

    if (invalid) {
        char* error_message = "Error";
        sendto(sockfd, error_message, strlen(error_message), 0, (const struct sockaddr *)&client_addr, client_addr_len);
    } else {
        sendto(sockfd, digits, strlen(digits), 0, (const struct sockaddr *)&client_addr, client_addr_len);
        sendto(sockfd, alphabets, strlen(alphabets), 0, (const struct sockaddr *)&client_addr, client_addr_len);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <PortNumber>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    create_socket(port);
    while (1)
    {
        receive_string();
        process_string();
    }
    
    close(sockfd);
    return 0;
}
