#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "socket_lib.h"
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDYELLOW "\033[1m\033[33m"
#define BOLDBLUE "\033[1m\033[34m"
#define BOLDCYAN "\033[1m\033[36m"
#define PORT 8071
#define BUFFER_SIZE 1024
#define IP_ADDRESS 192.168.XXX.XX //Replace this with the IP address of the server

int flag = 1;

void *receive_messages(void *socket) {
    int sock = *(int *)socket;
    char buffer[BUFFER_SIZE];
    while (1) {
        int valread = receive_message(sock, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            printf("Server disconnected\n");
            exit(0);
            break;
        }
        buffer[valread] = '\0';
        printf("%s\n", buffer);  // Print the received message
        // Check for win messages
        if (strcmp(buffer, "The host won the game!") == 0 ||
            strcmp(buffer, "The Trick or Treater won the game!") == 0) {
            flag = 0;
            printf("Game over: %s\n", buffer);
            
            close(sock);  // Close the socket before exiting
            exit(0);      // Exit the program
        }
    }
    return NULL;
}

int main() {
    int sock = client_init(IP_ADDRESS, PORT);
    if (sock < 0) {
        return -1;
    }

    char buffer[BUFFER_SIZE] = {0};

    // Prompt for username
    if (flag) {
        receive_message(sock, buffer, BUFFER_SIZE); // Read "Enter your username: "
        printf("%s", buffer);
        fgets(buffer, BUFFER_SIZE, stdin); // Get username from user
        send_message(sock, buffer); // Send username to server
    }

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, (void *)&sock);

    if (flag == 0) {
        pthread_join(receive_thread, NULL);
    }

    // Main loop for sending messages
    while (flag) {
        fgets(buffer, BUFFER_SIZE, stdin);
        printf(CYAN"Please wait for the other person to respond...."RESET);
        send_message(sock, buffer);
        memset(buffer, 0, BUFFER_SIZE);
    }

    // Cleanup (not reached due to exit)
    close(sock);
    pthread_join(receive_thread, NULL);
    return 0;
}
