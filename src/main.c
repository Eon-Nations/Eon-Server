#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "networking/networking.h"

#define PACKET_BUFFER_SIZE 20
#define INFINITE_LOOP 1
#define BUFFER_SIZE 2048

// Global variable to keep track of whether the server has been stopped by Ctrl+C
uint8_t server_stopped = 0;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

    while (INFINITE_LOOP) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        if (server_stopped) {
            break;
        }

        int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
        if (client_fd < 0) {
            printf("Error: Could not accept connection\n");
            return 1;
        }
        int pid = fork();
        if (pid == 0) {
            char* ip_str = malloc(INET6_ADDRSTRLEN);
            inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*)&client_addr), ip_str, INET6_ADDRSTRLEN);
            struct sockaddr_in* s = (struct sockaddr_in*)&client_addr;
            printf("\n\n|--------------------| ACCEPTED CONNECTION from %s:%d |--------------------|\n", ip_str, ntohs(s->sin_port));
            for (;;) {
                // Receive data
                uint8_t buffer[BUFFER_SIZE] = {0};
                int bytes_sent = recv(client_fd, buffer, BUFFER_SIZE, 0);
                if (bytes_sent < 0) {
                    printf("Error: Could not receive data\n");
                    return 1;
                }
                if (bytes_sent == 0) {
                    printf("Client disconnected\n");
                    break;
                }
                printf("Received %d bytes\n", bytes_sent);
                printf("Raw Data: ");
                for (int i = 0; i < bytes_sent; i++) {
                    printf(" 0x%x", buffer[i]);
                }
                printf("\n");
                packet_list_t packets;
                packets.packets = malloc(sizeof(packet_t) * PACKET_BUFFER_SIZE);
                packets.size = 0;
                read_packets(&packets, buffer, bytes_sent);
                print_packets(&packets);

                // Server processes packets and creates any packets to send back
                
                // Packets are sent back to the client at once
                free_stack_packet_list(&packets);
            }
            close(client_fd);
            return 0; // STATUS RETURNED TO PARENT PROCESS
        } else {
            close(client_fd); // Parent process isn't listening for connections
            printf("Forked process %d\n", pid);
            int status = waitid(P_ALL, 0, NULL, WEXITED);
            printf("Process %d exited with status %d\n", pid, status);
        }
    }
    close(socket_fd);
    freeaddrinfo(server_info);
    printf("Server Closed Succesfully!\n");
    return 0;
}