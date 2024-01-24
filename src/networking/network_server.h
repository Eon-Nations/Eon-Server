#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include "networking.h"
#include "packet_queue.h"
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define LISTEN_BACKLOG 50
#define SERVER_PORT 25565
#define BUFFER_SIZE 2048

typedef struct {
    int server_fd;
    struct sockaddr_storage* server_addr;
} network_server_t;

typedef struct {
    int client_fd;
    struct sockaddr_storage* client_addr;
    packet_queue_t* incoming_packets;
    packet_queue_t* outgoing_packets;
} client_connection_t;

network_server_t* create_mc_server();

void close_mc_server(network_server_t* server);

void accept_mc_connection(client_connection_t* client_conn, network_server_t* server);

packet_list_t* read_all_incoming_packets(client_connection_t* client_connection);

void close_client_connection(client_connection_t* client_connection);

#endif
