#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include "networking.h"
#include "packet_queue.h"
#include <sys/types.h>
#include <sys/socket.h>

#define LISTEN_BACKLOG 50
#define SERVER_PORT 25565

typedef struct {
    int server_fd;
    struct sockaddr_storage* server_addr;
} network_server_t;

network_server_t* create_mc_server();

client_connection_t* accept_network_server_conn(network_server_t* server);

void close_mc_server(network_server_t* server);

typedef struct {
    int client_fd;
    struct sockaddr_storage* client_addr;
    packet_queue_t* incoming_packets;
    packet_queue_t* outgoing_packets;
} client_connection_t;

client_connection_t* create_client_connection(struct sockaddr_storage* client_addr);

void close_client_connection(client_connection_t* client_connection);

#endif