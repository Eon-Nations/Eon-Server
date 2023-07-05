#include "network_server.h"

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

network_server_t* create_mc_server() {
    struct addrinfo* server_info = get_addr_info();
    if (server_info == NULL) {
        return NULL;
    }

    int socket_fd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (socket_fd < 0) {
        perror("Error: Could not create socket\n");
        return NULL;
    }

    int bind_status = bind(socket_fd, server_info->ai_addr, server_info->ai_addrlen);
    if (bind_status < 0) {
        perror("Error: Could not bind socket\n");
        close(socket_fd);
        return NULL;
    }

    int listen_status = listen(socket_fd, LISTEN_BACKLOG);
    if (listen_status < 0) {
        close(socket_fd);
        perror("Error: Could not listen on socket\n");
        return NULL;
    }

    printf("Listening on port %d\n", SERVER_PORT);

    network_server_t* server = malloc(sizeof(network_server_t));
    memset(server, 0, sizeof(network_server_t));
    server->server_fd = socket_fd;
    server->server_addr = (struct sockaddr_storage *) server_info;
    return server;
}


client_connection_t* accept_mc_connection(network_server_t* server) {
    struct sockaddr_storage* client_addr = malloc(sizeof(struct sockaddr_storage));
    socklen_t client_addr_size = sizeof(client_addr);
    client_connection_t* client_connection = malloc(sizeof(client_connection_t));
    memset(client_connection, 0, sizeof(client_connection_t));
    int client_fd = accept(server->server_fd, (struct sockaddr*)&client_addr, &client_addr_size);
    char* ip_str = malloc(INET6_ADDRSTRLEN);
    inet_ntop(client_addr->ss_family, get_in_addr((struct sockaddr*)client_addr), ip_str, INET6_ADDRSTRLEN);
    struct sockaddr_in* s = (struct sockaddr_in*)&client_addr;
    printf("\n\n|--------------------| ACCEPTED CONNECTION from %s:%d |--------------------|\n", ip_str, ntohs(s->sin_port));
    free(ip_str);
    client_connection->client_addr = client_addr;
    if (client_fd < 0) {
        perror("Error: Could not accept connection\n");
        return NULL;
    }
    client_connection->client_fd = client_fd;
    client_connection->incoming_packets = create_packet_queue();
    client_connection->outgoing_packets = create_packet_queue();
    return client_connection;
}

uint16_t read_all_incoming_packets(client_connection_t* client_connection) {
    packet_queue_t* incoming_packets = client_connection->incoming_packets;
    uint8_t* buffer = malloc(BUFFER_SIZE);
    int bytes_received = recv(client_connection->client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error: Could not receive data\n");
        return 0;
    }
    if (bytes_received == 0) {
        return 0;
    }
    packet_list_t packets;
    read_packets(&packets, buffer, bytes_received);
    for (uint32_t i = 0; i < packets.size; i++) {
        packet_t* packet = packets.packets[i];
        uint8_t status = queue_packet(incoming_packets, packet);
        if (status == PACKET_QUEUE_FULL) {
            printf("Error: Packet queue is full\n");
            free_stack_packet_list(&packets);
            free(buffer);
            return i + 1;
        }
    }
    free_stack_packet_list(&packets);
    free(buffer);
    return packets.size;
}

void close_mc_server(network_server_t* server) {
    close(server->server_fd);
    freeaddrinfo((struct addrinfo*) server->server_addr);
    free(server);
}

void close_client_connection(client_connection_t* client_connection) {
    close(client_connection->client_fd);
    free_packet_queue(client_connection->incoming_packets);
    free_packet_queue(client_connection->outgoing_packets);
    free(client_connection);
}