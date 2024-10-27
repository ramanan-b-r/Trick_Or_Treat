#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "llm.h"
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
#define BUFFER_SIZE 102400
#define MAX_CLIENTS 2

int client_sockets[MAX_CLIENTS];
char client_usernames[MAX_CLIENTS][BUFFER_SIZE];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int turn = 0;  // 0 indicates Client 1's turn, 1 indicates Client 2's turn
char* all_interaction;
int idx;
int cnt = 0;

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];
    int valread, client_index;

    // Get the client's index (0 or 1)
    pthread_mutex_lock(&clients_mutex);
    client_index = client_count++;
    client_sockets[client_index] = client_socket;
    pthread_mutex_unlock(&clients_mutex);

    // Ask for username
    send_message(client_socket, "Enter your username: ");
    valread = receive_message(client_socket, buffer, BUFFER_SIZE);
    buffer[valread - 1] = '\0'; // Remove newline character

    // Store the username
    pthread_mutex_lock(&clients_mutex);
    strcpy(client_usernames[client_index], buffer);
    pthread_mutex_unlock(&clients_mutex);

    if (client_index == 0) {
        send_message(client_socket, BOLDGREEN "You are the Host. You will have to figure out who the real human being is and give them the poisonous treat. Don't feed the zombie though ;)\n " RESET);
    } else {
        send_message(client_socket, BOLDGREEN "You are the Trick or Treater. Convince the evil host that you are a zombie and ensure you don't get fed a poisonous treat ;)\n" RESET);
    }

    printf("%s has joined the chat.\n", buffer);

    // Main communication loop
    while (1) {
        // Wait for the client's turn
        while (turn != client_index);

        // Prompt the client to enter a message
        send_message(client_socket, "============================================\n");
        send_message(client_socket, all_interaction);
        if (client_index == 0) {
            if(cnt == 10)
                send_message(client_socket, BOLDGREEN "Your turn and you MUST select the player to give the candy to in this turn. Else you lose the game. (Enter GUESS 1 or GUESS 2) to end the game and choose the player to give the candy to: " RESET);
            else
                send_message(client_socket, BOLDGREEN "Your turn. Either ask another question or enter GUESS 1 or GUESS 2 to end the game and choose the player to give the candy to:\nEnter your message: " RESET);
            valread = receive_message(client_socket, buffer, BUFFER_SIZE);
            if (valread <= 0) break; // If client disconnected, exit loop
            buffer[valread] = '\0';
            printf("%s", buffer);
            if (strcmp(buffer, "GUESS 1\n") == 0 || strcmp(buffer, "GUESS 2\n") == 0) {
                if ((int)(buffer[6] - '0') == idx) {
                    send_message(client_socket, RED "The host won the game!" RESET);
                    int other_client_index = (client_index + 1) % 2;
                    send_message(client_sockets[other_client_index], RED "The host won the game!" RESET);
                    exit(0);
                } else {
                    send_message(client_socket, RED "The Trick or Treater won the game!" RESET);
                    int other_client_index = (client_index + 1) % 2;
                    send_message(client_sockets[other_client_index], RED "The Trick or Treater won the game!" RESET);
                    exit(0);
                }
            }
            else if(cnt == 10) {
                send_message(client_socket, RED "The Trick or Treater won the game!" RESET);
                int other_client_index = (client_index + 1) % 2;
                send_message(client_sockets[other_client_index], RED "The Trick or Treater won the game!" RESET);
                exit(0);
            }
        } else {
            send_message(client_socket, "Your turn:");
            valread = receive_message(client_socket, buffer, BUFFER_SIZE);
            if (valread <= 0) break; // If client disconnected, exit loop
            buffer[valread] = '\0';
        }

        pthread_mutex_lock(&clients_mutex);
        int other_client_index = (client_index + 1) % 2;
        char message[BUFFER_SIZE];

        if (client_index == 0) {
            snprintf(message, BUFFER_SIZE, BOLDCYAN "Host: %s" RESET, buffer);
            all_interaction = (char *)realloc(all_interaction, sizeof(char) * BUFFER_SIZE);
            strcat(all_interaction, message);
            all_interaction = (char *)realloc(all_interaction, sizeof(char) * BUFFER_SIZE);
            strcat(all_interaction, "\n");
        } else {
            char* llm_out = query_llm(all_interaction);
            printf("%s %s\n", all_interaction, llm_out);
            if (idx == 1) {
                snprintf(message, BUFFER_SIZE, BOLDYELLOW "Trick or Treater 1: %s\n" RESET, buffer);
                all_interaction = (char *)realloc(all_interaction, sizeof(char) * BUFFER_SIZE);
                strcat(all_interaction, message);
                snprintf(message, BUFFER_SIZE, BOLDYELLOW "Trick or Treater 2: %s\n" RESET, llm_out);
                all_interaction = (char *)realloc(all_interaction, sizeof(char) * BUFFER_SIZE);
                strcat(all_interaction, message);
            } else {
                snprintf(message, BUFFER_SIZE, BOLDYELLOW "Trick or Treater 1: %s\n" RESET, llm_out);
                all_interaction = (char *)realloc(all_interaction, sizeof(char) * BUFFER_SIZE);
                strcat(all_interaction, message);
                snprintf(message, BUFFER_SIZE, BOLDYELLOW "Trick or Treater 2: %s\n" RESET, buffer);
                all_interaction = (char *)realloc(all_interaction, sizeof(char) * BUFFER_SIZE);
                strcat(all_interaction, message);
            }
            free(llm_out);
            cnt++;
        }
        pthread_mutex_unlock(&clients_mutex);

        // Switch turn to the other client
        turn = other_client_index;
    }

    // Clean up when a client disconnects
    pthread_mutex_lock(&clients_mutex);
    client_sockets[client_index] = 0;
    client_count--;
    printf("%s has left the chat.\n", client_usernames[client_index]);
    pthread_mutex_unlock(&clients_mutex);
    close(client_socket);
    free(arg);
    return NULL;
}

int main() {
    all_interaction = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    idx = rand() % 2 + 1;

    int server_fd = server_init(PORT, MAX_CLIENTS);
    if (server_fd < 0) {
        return -1;
    }

    printf("Server waiting for connections...\n");

    while (1) {
        connection_t *new_connection = server_accept(server_fd);
        if (new_connection == NULL) {
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, &(new_connection->socket));
    }

    close(server_fd);
    return 0;
}