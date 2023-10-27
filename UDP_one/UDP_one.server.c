#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
  struct sockaddr_in server_addr, client_addr;
  int server_fd, n;
  socklen_t addr_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];

  // Create socket
  if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  memset(&client_addr, 0, sizeof(client_addr));

  // Server address configuration
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind the socket with the server address
  if (bind(server_fd, (const struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", PORT);

  while (1) {
    // Receive message from client
    n = recvfrom(server_fd, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&client_addr, &addr_len);
    buffer[strcspn(buffer, "\n")] = 0;

    // Print the received message
    printf("Client: %s\n", buffer);
    if (strcmp(buffer, "exit") == 0) {
      break;
    }
  }

  close(server_fd);
  return 0;
}
