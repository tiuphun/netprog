#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#define BUFFER_SIZE 256

// Function to resolve a domain name to its IP address
void resolve_domain(const char* domain) {
    struct addrinfo hints, *res, *p;
    char ip[INET6_ADDRSTRLEN];  // buffer to hold the IP address

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // either IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(domain, NULL, &hints, &res);
    if (status != 0) {
        printf("Not found information\n");
        return;
    }

    printf("Official IP: ");
    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;

        // IPv4
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        }
        // IPv6
        else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // Convert binary to string form
        inet_ntop(p->ai_family, addr, ip, sizeof ip);
        printf("%s\n", ip);
    }

    freeaddrinfo(res);  // free the linked list
}

// Function to resolve an IP address to its domain name
void resolve_ip(const char* ip_str) {
    struct sockaddr_in sa;
    struct hostent *he;

    // Convert IP string to binary form
    if (inet_pton(AF_INET, ip_str, &(sa.sin_addr)) != 1) {
        printf("Invalid address\n");
        return;
    }

    // Perform reverse DNS lookup
    he = gethostbyaddr(&sa.sin_addr, sizeof(sa.sin_addr), AF_INET);
    if (he == NULL) {
        printf("Not found information\n");
        return;
    }

    printf("Official name: %s\n", he->h_name);

    if (he->h_aliases[0] != NULL) {
        printf("Alias name:\n");
        for (char **alias = he->h_aliases; *alias != NULL; alias++) {
            printf("%s\n", *alias);
        }
    }
}

// Main function to handle user input
int main() {
    char input[BUFFER_SIZE];

    while (1) {
        // Prompt the user for input
        printf("Enter a domain name or IP address (leave blank to exit): ");
        fgets(input, BUFFER_SIZE, stdin);

        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';

        // Exit the loop if the input is blank
        if (strlen(input) == 0) {
            break;
        }

        // Check if the input is a valid IP address or domain name
        struct sockaddr_in sa;
        int is_ip = inet_pton(AF_INET, input, &(sa.sin_addr));

        // If input is a valid IP address
        if (is_ip == 1) {
            resolve_ip(input);
        } else {
            resolve_domain(input);
        }
    }

    printf("Program terminated.\n");
    return 0;
}
