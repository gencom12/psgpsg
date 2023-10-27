#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" // Use the server's IP address
#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
  struct sockaddr_in server_addr;
  int client_fd;
  char buffer[BUFFER_SIZE];

  // Create socket
  if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));

  // Server address configuration
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(PORT);

  printf("Enter your message (type 'exit' to quit):\n");

  while (1) {
    // Read message from user
    fgets(buffer, sizeof(buffer), stdin);

    // Send message to server
    sendto(client_fd, buffer, strlen(buffer), 0,
           (const struct sockaddr *)&server_addr, sizeof(server_addr));

    // Check if user wants to exit
    if (strncmp(buffer, "exit", 4) == 0) {
      printf("Goodbye!\n");
      break;
    }
  }

  close(client_fd);
  return 0;
}
