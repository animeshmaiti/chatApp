#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 2000

int main() {
    int serversockfd, clientsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[1024];
    char message[1024];
    socklen_t clientaddr_size;

    // Create a socket
    serversockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serversockfd == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY; // localhost
    serveraddr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(serversockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serversockfd, 1) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Communication loop
    while (1) {
        // Accept a connection
        clientaddr_size = sizeof(clientaddr);
        clientsockfd = accept(serversockfd, (struct sockaddr*)&clientaddr, &clientaddr_size);
        if (clientsockfd < 0) {
            perror("accept failed");
            continue;
        }

        // Communication loop with client
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            // Receive message from client
            if (recv(clientsockfd, buffer, sizeof(buffer), 0) == 0) {
                printf("Client disconnected\n");
                break;
            }

            // Print client message
            printf("Client: %s\n", buffer);

            // Check for exit command
            if (strcasecmp(buffer, "END") == 0) {
                send(clientsockfd, "BYE", strlen("BYE"), 0);
                break;
            }

            // Get server message from user
            printf("Server: ");
            fgets(message, sizeof(message), stdin);
            message[strcspn(message, "\n")] = '\0'; // Remove trailing newline

            // Send message to client
            send(clientsockfd, message, strlen(message), 0);
            printf("Waiting for client to reply...\n");
        }

        // Close client socket
        close(clientsockfd);
    }

    // Close server socket
    close(serversockfd);

    return 0;
}
