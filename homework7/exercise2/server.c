#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAX_ATTEMPTS 5
#define ACCOUNT_FILE "account.txt"

typedef struct {
    char userID[50];
    char password[50];
    int status; // 1: Active, 0: Locked
    int attempts;
} Account;

Account accounts[MAX_CLIENTS];
int account_count = 0;
struct pollfd clients[MAX_CLIENTS];
int client_count = 0;

// Load accounts from the file
void load_accounts() {
    FILE *file = fopen(ACCOUNT_FILE, "r");
    if (file == NULL) {
        perror("Failed to open account file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%s %s %d", accounts[account_count].userID, accounts[account_count].password, &accounts[account_count].status) != EOF) {
        accounts[account_count].attempts = 0; // Reset attempts
        account_count++;
    }
    fclose(file);
}

// Save updated accounts back to the file
void save_accounts() {
    FILE *file = fopen(ACCOUNT_FILE, "w");
    if (file == NULL) {
        perror("Failed to save account file");
        return;
    }

    for (int i = 0; i < account_count; i++) {
        fprintf(file, "%s %s %d\n", accounts[i].userID, accounts[i].password, accounts[i].status);
    }
    fclose(file);
}

// Find an account by UserID
Account *find_account(const char *userID) {
    for (int i = 0; i < account_count; i++) {
        if (strcmp(accounts[i].userID, userID) == 0) {
            return &accounts[i];
        }
    }
    return NULL;
}

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    char userID[50], password[50];
    int bytes_received;

    printf("Handling client socket %d\n", client_sock); // Debug log

    // Receive UserID
    memset(buffer, 0, BUFFER_SIZE);
    bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        printf("Client disconnected or error receiving UserID\n");
        return;
    }
    buffer[bytes_received] = '\0';
    // printf("Received UserID: %s\n", buffer); // Debug log
    strcpy(userID, buffer);

    // Receive Password
    memset(buffer, 0, BUFFER_SIZE);
    bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
        printf("Client disconnected or error receiving Password\n");
        return;
    }
    buffer[bytes_received] = '\0';
    // printf("Received Password: %s\n", buffer); // Debug log
    strcpy(password, buffer);

    // Authenticate User
    Account *account = find_account(userID);
    if (account == NULL) {
        send(client_sock, "Account not found\n", 18, 0);
    } else if (account->status == 0) {
        send(client_sock, "Account is locked\n", 18, 0);
    } else if (strcmp(account->password, password) == 0) {
        account->attempts = 0;
        send(client_sock, "Login successful\n", 18, 0);
    } else {
        account->attempts++;
        if (account->attempts >= MAX_ATTEMPTS) {
            account->status = 0; // Lock the account after 5 failed attempts
            save_accounts();
            send(client_sock, "Account is locked due to too many failed attempts\n", 50, 0);
        } else {
            send(client_sock, "Invalid password\n", 17, 0);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_sock, client_sock;
    struct sockaddr_in server_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_in);

    // Load accounts from file
    load_accounts();

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking mode
    // fcntl(server_sock, F_SETFL, O_NONBLOCK);

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Add the server socket to the poll set
    struct pollfd pfd;
    pfd.fd = server_sock;
    pfd.events = POLLIN;
    clients[client_count++] = pfd;

    printf("Server is listening on port %d\n", port);

    // Use poll() to handle multiple clients
    while (1) {
        int poll_count = poll(clients, client_count, -1);
        if (poll_count == -1) {
            perror("poll() error");
            break;
        }

        for (int i = 0; i < client_count; i++) {
            if (clients[i].revents & POLLIN) {
                if (clients[i].fd == server_sock) {
                    // Accept new connection
                    client_sock = accept(server_sock, (struct sockaddr *)&server_addr, &client_addr_len);
                    if (client_sock < 0) {
                        perror("Accept failed");
                    } else {
                        // Add client socket to the poll set
                        struct pollfd new_client;
                        new_client.fd = client_sock;
                        new_client.events = POLLIN;
                        clients[client_count++] = new_client;
                    }
                } else {
                    // Handle client request
                    handle_client(clients[i].fd);
                    // close(clients[i].fd);
                    clients[i] = clients[--client_count]; // Remove client from poll set
                }
            }
        }
    }

    close(server_sock);
    return 0;
}
