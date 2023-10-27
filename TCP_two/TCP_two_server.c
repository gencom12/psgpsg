#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT 8080

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  char buffer[MAX_BUFFER_SIZE];

  // Create server socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Prepare server address structure
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(SERVER_PORT);

  // Bind the socket to the specified IP and port
  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) < 0) {
    perror("Binding failed");
    exit(EXIT_FAILURE);
  }

  // Start listening for incoming connections
  if (listen(server_socket, 5) < 0) {
    perror("Listening failed");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", SERVER_PORT);

  // Accept the first client
  socklen_t client_address_size = sizeof(client_address);
  client_socket = accept(server_socket, (struct sockaddr *)&client_address,
                         &client_address_size);
  if (client_socket < 0) {
    perror("Accept failed");
    exit(EXIT_FAILURE);
  }

  printf("New client connected: %s\n", inet_ntoa(client_address.sin_addr));

  // Enter a loop for two-way communication
  while (1) {
    // Receive message from the client
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
      perror("Receiving failed");
      exit(EXIT_FAILURE);
    }

    if (bytes_received == 0) {
      printf("Client disconnected\n");
      break;
    }

    printf("Received message from client: %s\n", buffer);

    // Process data or perform operations here

    // Send a reply to the client
    printf("Enter a reply to send (or 'exit' to quit): ");
    fgets(buffer, sizeof(buffer), stdin);

    // Remove newline character from the input
    buffer[strcspn(buffer, "\n")] = 0;

    // Check if the server wants to exit
    if (strcmp(buffer, "exit") == 0)
      break;

    // Send the reply to the client
    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
      perror("Sending failed");
      exit(EXIT_FAILURE);
    }
  }

  // Close the client socket
  close(client_socket);

  // Close the server socket
  close(server_socket);

  return 0;
}
