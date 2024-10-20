#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void resolve_domain(const char* domain) {
    struct addrinfo hints, *res, *p;
    char ip[INET6_ADDRSTRLEN];  // buffer to hold the IP address

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(domain, NULL, &hints, &res);
    if (status != 0) {
        printf("Not found information\n");
        return;
    }

    printf("Official IP: ");
    for(p = res; p != NULL; p = p->ai_next) {
        void *addr;
        if (p->ai_family == AF_INET) {  // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {  // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        inet_ntop(p->ai_family, addr, ip, sizeof ip);
        printf("%s\n", ip);
    }

    freeaddrinfo(res);  // free the linked list
}

void resolve_ip(const char* ip_str) {
    struct sockaddr_in sa;
    struct hostent *he;

    // validate the IP
    if (inet_pton(AF_INET, ip_str, &(sa.sin_addr)) != 1) {
        printf("Invalid address\n");
        return;
    }

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain or IP>\n", argv[0]);
        return 1;
    }

    // Check if the input is an IP address or domain name
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, argv[1], &(sa.sin_addr)) == 1) {
        // If valid IPv4 address, resolve it to a domain name
        resolve_ip(argv[1]);
    } else {
        // Else, treat it as a domain name
        resolve_domain(argv[1]);
    }

    return 0;
}
