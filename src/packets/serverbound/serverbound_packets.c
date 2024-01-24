#include "serverbound_packets.h"

handshake_packet_t* parse_handshake_packet(packet_t* packet) {
    handshake_packet_t* hp = malloc(sizeof(handshake_packet_t));
    uint32_t bytes_read = 0, server_length = 0;
    hp->protocol_version = read_varint(packet->data, packet->size, &bytes_read);
    uint32_t server_address_length = read_varint(packet->data + bytes_read, packet->size, &server_length);
    bytes_read += server_length;
    hp->server_address = malloc(server_address_length + 1);
    memcpy(hp->server_address, packet->data + bytes_read, server_address_length);
    hp->server_address[server_address_length] = '\0';
    bytes_read += server_address_length;
    hp->server_port = read_ushort(packet->data + bytes_read, &bytes_read);
    hp->next_state = read_varint(packet->data + bytes_read, packet->size, &bytes_read);
    return hp;
}

void print_handshake_packet(handshake_packet_t* hp) {
    printf("Handshake Packet:\n");
    printf("Protocol Version: %d\n", hp->protocol_version);
    printf("Server Address: %s\n", hp->server_address);
    printf("Server Port: %d\n", hp->server_port);
    printf("Next State: %d\n", hp->next_state);
}

void free_handshake_packet(handshake_packet_t* hp) {
    free(hp->server_address);
    free(hp);
}


