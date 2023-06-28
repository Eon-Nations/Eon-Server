#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include "networking/networking.h"

#define SERVER_PORT 25565
#define LISTEN_BACKLOG 50
#define INFINITE_LOOP 1
#define BUFFER_SIZE 2048

// Global variable to keep track of whether the server has been stopped by Ctrl+C
uint8_t server_stopped = 0;


struct addrinfo* get_addr_info() {
    const char* port = "25565";
    struct addrinfo connection_vars = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_flags = AI_PASSIVE,
        .ai_protocol = 0,
        .ai_addrlen = 0,
        .ai_canonname = NULL,
        .ai_addr = NULL,
        .ai_next = NULL
    };
    struct addrinfo* server_info;

    int status = getaddrinfo(NULL, port, &connection_vars, &server_info);
    if (status < 0) {
        printf("Error: %s\n", gai_strerror(status));
        return NULL;
    }
    return server_info;
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



int main(int argc, char** argv) {
    printf("Starting...\n");
    if (argc > 0) {
        printf("Arguments:\n");
        for (int i = 0; i < argc; i++) {
            printf("\t%s\n", argv[i]);
        }
    }
    listen_for_death();
    kill_dead_processes();
    struct addrinfo* server_info = get_addr_info();
    if (server_info == NULL) {
        return 1;
    }

    int socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (socket_fd < 0) {
        perror("Error: Could not create socket\n");
        return 1;
    }

    int bind_status = bind(socket_fd, server_info->ai_addr, server_info->ai_addrlen);
    if (bind_status < 0) {
        perror("Error: Could not bind socket\n");
        return 1;
    }

    int listen_status = listen(socket_fd, LISTEN_BACKLOG);
    if (listen_status < 0) {
        perror("Error: Could not listen on socket\n");
        return 1;
    }

    printf("Listening on port %d\n", SERVER_PORT);

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
            printf("Accepted connection\n");
            uint8_t buffer[BUFFER_SIZE] = {0};
            int bytes_sent = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_sent < 0) {
                printf("Error: Could not receive data\n");
                return 1;
            }
            printf("Received %d bytes\n", bytes_sent);
            printf("Raw Data: ");
            for (int i = 0; i < bytes_sent; i++) {
                printf(" %x", buffer[i]);
            }
            printf("\n");

            uint32_t packet_bytes_read = 0;
            packet_t* packet = read_packet(buffer, bytes_sent, &packet_bytes_read);
            print_packet(packet);
            free_packet(packet);
            printf("Rest of Raw Data: ");
            packet_bytes_read -= 1;
            for (int i = packet_bytes_read; i < bytes_sent; i++) {
                printf(" 0x%x", buffer[i]);
            }
            printf("\n");
            packet = read_packet(buffer + packet_bytes_read, bytes_sent - packet_bytes_read, &packet_bytes_read);
            print_packet(packet);
            free_packet(packet);
            return 0; // STATUS RETURNED TO PARENT PROCESS
        } else {
            printf("Forked process %d\n", pid);
            int status = waitid(P_ALL, 0, NULL, WEXITED);
            printf("Process %d exited with status %d\n", pid, status);
        }
        close(client_fd); // Added to fix implicit declaration of function 'close'
    }
    close(socket_fd);
    freeaddrinfo(server_info);
    printf("Server Closed Succesfully!\n");
    return 0;
}