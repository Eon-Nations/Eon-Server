#include "handshake.h"

handshake_packet_t* parse_packet(packet_t* packet) {
    handshake_packet_t* handshake_packet = malloc(sizeof(handshake_packet_t));
    uint32_t bytes_read = 0;
    int32_t protocol_version = read_varint(packet->data, packet->size, &bytes_read);
    int32_t server_address_length = read_varint(packet->data + bytes_read, packet->size, &bytes_read);
    handshake_packet->server_address = malloc(server_address_length + 1);
    memcpy(handshake_packet->server_address, packet->data + bytes_read, server_address_length);
    handshake_packet->server_address[server_address_length] = '\0';
    bytes_read += server_address_length;
    handshake_packet->server_port = read_ushort(packet->data + bytes_read, &bytes_read);
    handshake_packet->next_state = read_varint(packet->data + bytes_read, packet->size, &bytes_read);
    return handshake_packet;
}