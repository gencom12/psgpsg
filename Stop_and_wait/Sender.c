// STOP AND WAIT CLIENT

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_PACKET_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

typedef struct {
  int sequence_number;
  char data[MAX_PACKET_SIZE];
} Packet;

void send_packet(Packet *packet, int server_socket) {
  send(server_socket, packet, sizeof(Packet), 0);
  printf("Client: Sent packet with sequence number %d: %s\n",
         packet->sequence_number, packet->data);
}

bool receive_ack(int expected_sequence_number, int server_socket) {
  int received_ack;
  recv(server_socket, &received_ack, sizeof(int), 0);

  if (received_ack == expected_sequence_number) {
    printf("Client: Received ACK for sequence number: %d\n", received_ack);
    return true;
  } else {
    printf("Client: Received NAK (ACK with unexpected sequence number)\n");
    return false;
  }
}

int main() {
  int client_socket;
  struct sockaddr_in server_addr;

  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("Client: Socket creation failed");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  server_addr.sin_port = htons(SERVER_PORT);

  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) == -1) {
    perror("Client: Connection failed");
    exit(EXIT_FAILURE);
  }

  int sequence_number = 0;
  Packet packet;

  while (true) {
    printf("Enter data to send (or 'exit' to quit): ");
    fgets(packet.data, MAX_PACKET_SIZE, stdin);

    packet.sequence_number = sequence_number;
    send_packet(&packet, client_socket);
    if (strcmp(packet.data, "exit\n") == 0) {
      break;
    }
    printf("Client: Waiting for ACK...\n");
    while (!receive_ack(sequence_number, client_socket)) {
      printf("Client: Resending packet...\n");
      send_packet(&packet, client_socket);
    }

    sequence_number = 1 - sequence_number; // Toggle sequence number
  }

  close(client_socket);

  return 0;
}