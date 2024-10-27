#include "socket_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int server_init(int port, int max_clients) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, max_clients) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

connection_t* server_accept(int server_socket) {
    connection_t *connection = malloc(sizeof(connection_t));
    socklen_t addr_len = sizeof(connection->address);

    connection->socket = accept(server_socket, (struct sockaddr *)&connection->address, &addr_len);
    if (connection->socket < 0) {
        perror("Accept failed");
        free(connection);
        return NULL;
    }

    return connection;
}

int client_init(const char *server_ip, int port) {
    int client_socket;
    struct sockaddr_in address;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(client_socket);
        return -1;
    }

    if (connect(client_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Connection Failed");
        close(client_socket);
        return -1;
    }

    return client_socket;
}

int send_message(int socket, const char *message) {
    return send(socket, message, strlen(message), 0);
}

int receive_message(int socket, char *buffer, int size) {
    return recv(socket, buffer, size, 0);
}

void* default_handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[valread] = '\0';
        printf("Client: %s\n", buffer);
        send(client_socket, buffer, valread, 0);
    }

    close(client_socket);
    free(arg);
    return NULL;
}