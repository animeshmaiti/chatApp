#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 2000

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char buffer[1024];
    char message[1024];

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY; // localhost
    serveraddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    // Communication loop
    while (1) {
        // Get message from user
        printf("Client: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0'; // Remove trailing newline

        // Send message to server
        send(sockfd, message, strlen(message), 0);
        printf("Waiting for server to reply...\n");
        
        memset(buffer, 0, sizeof(buffer));
        // Receive message from server
        if (recv(sockfd, buffer, sizeof(buffer), 0) == 0) {
            printf("Server disconnected\n");
            break;
        }

        // Print server message
        printf("Server: %s\n", buffer);

        // Check for exit command
        if (strcasecmp(buffer, "BYE") == 0) {
            break;
        }
    }

    // Close socket and resources
    close(sockfd);

    return 0;
}
