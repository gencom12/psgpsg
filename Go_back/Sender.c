// GO BACK CLIENT

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WINDOW_SIZE 5

int main() {
  int client_fd;
  struct sockaddr_in server_addr;

  // Create socket
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);

  // Convert IPv4 and IPv6 addresses from text to binary form
  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
    perror("Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  // Connect to the server
  if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }

  printf("Connected to the server...\n");

  int packet = 0;
  int last_packet = 20;

  while (packet <= last_packet) {
    // Send packets with a sliding window mechanism
    for (int i = 0; i < WINDOW_SIZE && packet <= last_packet; i++) {
      printf("Sending packet: %d\n", packet);
      send(client_fd, &packet, sizeof(packet), 0);
      packet++;
    }

    // Receive ACKs for the sent packets
    for (int i = 0; i < WINDOW_SIZE && packet <= last_packet; i++) {
      int ack_packet;
      recv(client_fd, &ack_packet, sizeof(ack_packet), 0);
      printf("Received ACK for packet: %d\n", ack_packet);
    }
  }

  // Signal end of transmission
  packet = -1;
  send(client_fd, &packet, sizeof(packet), 0);
  close(client_fd);

  return 0;
}