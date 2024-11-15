#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAX_ATTEMPTS 5

typedef struct {
    char userID[50];
    char password[50];
    int status; // 1: Active, 0: Locked
    int attempts;
} Account;

Account accounts[MAX_CLIENTS];
int account_count = 0;
pthread_mutex_t lock;

// Load accounts from the file
void load_accounts() {
    FILE *file = fopen("account.txt", "r");
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
    pthread_mutex_lock(&lock);
    FILE *file = fopen("account.txt", "w");
    if (file == NULL) {
        perror("Failed to save account file");
        pthread_mutex_unlock(&lock);
        return;
    }

    for (int i = 0; i < account_count; i++) {
        fprintf(file, "%s %s %d\n", accounts[i].userID, accounts[i].password, accounts[i].status);
    }
    fclose(file);
    pthread_mutex_unlock(&lock);
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

// Thread function to handle a client
void *client_handler(void *arg) {
    int client_sock = *(int *)arg;
    free(arg); // Free dynamically allocated memory for client socket
    char buffer[BUFFER_SIZE];
    char userID[50], password[50];

    while (1) {
        // Receive UserID
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            close(client_sock);
            pthread_exit(NULL);
        }
        buffer[bytes_received] = '\0';
        strcpy(userID, buffer);

        // Receive Password
        memset(buffer, 0, BUFFER_SIZE);
        bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            close(client_sock);
            pthread_exit(NULL);
        }
        buffer[bytes_received] = '\0';
        strcpy(password, buffer);

        // Authenticate User
        Account *account = find_account(userID);
        if (account == NULL) {
            send(client_sock, "Account not found\n", 18, 0);
        } else if (account->status == 0) {
            send(client_sock, "Account is locked\n", 18, 0);
        } else if (strcmp(account->password, password) == 0) {
            pthread_mutex_lock(&lock);
            account->attempts = 0;
            pthread_mutex_unlock(&lock);
            send(client_sock, "Login successful\n", 18, 0);
            break;
        } else {
            pthread_mutex_lock(&lock);
            account->attempts++;
            if (account->attempts >= MAX_ATTEMPTS) {
                account->status = 0;
                save_accounts();
                send(client_sock, "Account is locked due to too many failed attempts\n", 50, 0);
            } else {
                send(client_sock, "Invalid password\n", 17, 0);
            }
            pthread_mutex_unlock(&lock);
        }
    }

    close(client_sock);
    printf("Client disconnected.\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port_Number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    pthread_mutex_init(&lock, NULL);
    load_accounts();

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

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

    printf("Server is listening on port %d\n", port);

    // Accept clients
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        int *new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, new_sock) != 0) {
            perror("Thread creation failed");
            free(new_sock);
            close(client_sock);
        }
    }

    close(server_sock);
    pthread_mutex_destroy(&lock);
    return 0;
}
