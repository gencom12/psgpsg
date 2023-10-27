#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_IP "10.1.22.246"
#define SERVER_PORT 8080

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char message[MAX_BUFFER_SIZE];
    char buffer[MAX_BUFFER_SIZE];

    // Create client socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Enter a loop for two-way communication
    while (1) {
        printf("Enter a message to send (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        // Remove newline character from the input
        message[strcspn(message, "\n")] = 0;

        // Send the message to the server
        if (send(client_socket, message, strlen(message), 0) < 0) {
            perror("Sending failed");
            exit(EXIT_FAILURE);
        }

        // Check if the client wants to exit
        if (strcmp(message, "exit") == 0)
            break;

        // Receive the server's reply
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            perror("Receiving failed");
            exit(EXIT_FAILURE);
        }

        printf("Server reply: %s\n", buffer);
    }

    // Close the client socket
    close(client_socket);

    return 0;
}