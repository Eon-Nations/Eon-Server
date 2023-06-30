#ifndef HANDSHAKE_PACKET_H
#define HANDSHAKE_PACKET_H

#include <stdint.h>
#include "../../networking/networking.h"

typedef struct {
    int32_t protocol_version;
    char* server_address;
    uint16_t server_port;
    int32_t next_state;
} handshake_packet_t;

handshake_packet_t* parse_packet(packet_t* packet);

void print_handshake_packet(handshake_packet_t* packet);

void free_handshake_packet(handshake_packet_t* packet);

#endif