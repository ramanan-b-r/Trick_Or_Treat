#ifndef SOCKET_LIB_H
#define SOCKET_LIB_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    struct sockaddr_in address;
} connection_t;

// Function to initialize a server
int server_init(int port, int max_clients);

// Function to accept a client connection
connection_t* server_accept(int server_socket);

// Function to initialize a client
int client_init(const char *server_ip, int port);

// Function to send a message
int send_message(int socket, const char *message);

// Function to receive a message
int receive_message(int socket, char *buffer, int size);

// Renamed default client handler function
void* default_handle_client(void *arg);

#endif // SOCKET_LIB_H