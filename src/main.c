#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "networking/networking.h"
#include "networking/network_server.h"

#define PACKET_BUFFER_SIZE 20
#define INFINITE_LOOP 1

// Global variable to keep track of whether the server has been stopped by Ctrl+C
uint8_t server_stopped = 0;

void sigchld_handler(__attribute__((unused)) int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void kill_dead_processes() {
    struct sigaction sa = {
        .sa_handler = sigchld_handler,
        .sa_flags = SA_RESTART
    };
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Error: Could not kill dead processes\n");
    }
}

void kill_handler(int signal_num) {
    server_stopped = signal_num;
}

void listen_for_death() {
    struct sigaction sa = {
        .sa_handler = kill_handler,
        .sa_flags = SA_RESTART
    };
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error: Could not listen for death\n");
    }
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
    listen_for_death();
    kill_dead_processes();

    network_server_t* server = create_mc_server();
    if (server == NULL) {
        return 1;
    }
    while (!server_stopped) {
        client_connection_t* client = malloc(sizeof(client_connection_t));
        accept_mc_connection(client, server);
        if (client->client_fd != -1) {
            packet_list_t* packets = read_all_incoming_packets(client);
            for (uint32_t i = 0; i < packets->size; i++) {
                packet_t* packet = packets->packets[i];
                print_packet(packet);
            }
            free_packet_list(packets);
        }
        close_client_connection(client);
    }
    close_mc_server(server);
    printf("Server Closed Succesfully!\n");
    return 0;
}
