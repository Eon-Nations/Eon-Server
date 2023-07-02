#include "network_server.h"

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
        return NULL;
    }

    int listen_status = listen(socket_fd, LISTEN_BACKLOG);
    if (listen_status < 0) {
        perror("Error: Could not listen on socket\n");
        return NULL;
    }

    printf("Listening on port %d\n", SERVER_PORT);

    network_server_t* server = malloc(sizeof(network_server_t));
    memset(server, 0, sizeof(network_server_t));
    server->socket_fd = socket_fd;
    server->server_info = server_info;
    return server;
}


client_connection_t* create_client_connection(struct sockaddr_storage* client_addr) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    client_connection_t* client_connection = malloc(sizeof(client_connection_t));
    memset(client_connection, 0, sizeof(client_connection_t));
    int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_addr_size);
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

void close_client_connection(client_connection_t* client_connection) {
    close(client_connection->client_fd);
    free_packet_queue(client_connection->incoming_packets);
    free_packet_queue(client_connection->outgoing_packets);
    free(client_connection);
}