// STOP AND WAIT SERVER

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PACKET_SIZE 1024
#define SERVER_PORT 8080

typedef struct {
  int sequence_number;
  char data[MAX_PACKET_SIZE];
} Packet;
// packet structure

void receive(Packet *packet, int client_socket) {
  recv(client_socket, packet, sizeof(Packet),
       0); // getting the packets through TCP
  printf("Server: Received packet with sequence number %d: %s\n",
         packet->sequence_number, packet->data);
  packet->data[strcspn(packet->data, "\n")] = 0;
  if (strcmp(packet->data, "exit") == 0) {
    printf("Server: Exiting...\n");
    exit(0);
  }
}

void send_ack(int sequence_number, int client_socket) {
  send(client_socket, &sequence_number, sizeof(int), 0);
  printf("Server: Sent ACK for sequence number: %d\n", sequence_number);
}

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("Server: Socket creation failed");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(SERVER_PORT);

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    perror("Server: Bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_socket, 1) == -1) {
    perror("Server: Listen failed");
    exit(EXIT_FAILURE);
  }

  printf("Server: Listening on port %d...\n", SERVER_PORT);

  client_socket =
      accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket == -1) {
    perror("Server: Accept failed");
    exit(EXIT_FAILURE);
  }

  Packet received_packet;

  int expected_sequence_number = 0;

  while (true) {
    receive(&received_packet, client_socket);

    if (received_packet.sequence_number == expected_sequence_number) {
      send_ack(expected_sequence_number, client_socket);
      expected_sequence_number =
          1 - expected_sequence_number; // Toggle sequence number
    } else {
      printf("Server: Discarded packet with unexpected sequence number\n");
    }
  }

  close(client_socket);
  close(server_socket);

  return 0;
}