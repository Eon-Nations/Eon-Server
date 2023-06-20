#include "packet_reader.h"

handshake_packet* read_hp(packet* p) {
    handshake_packet* hp = (handshake_packet*) malloc(sizeof(handshake_packet));
    int num_read = 0;
    hp->protocol_version = read_varint(p->packet_data, &num_read);
    int server_address_len = read_varint(p->packet_data, &num_read);
    hp->server_address = (char*) malloc(server_address_len);
    memcpy(hp->server_address, p->packet_data + num_read, server_address_len);
    num_read += server_address_len;
    hp->server_port = read_ushort(p->packet_data + num_read);
    num_read += 2;
    hp->next_state = read_varint(p->packet_data, &num_read);
    return hp;
}

packet* read_packet(char* buffer) {
    packet* p = (packet*) malloc(sizeof(packet));
    int num_read = 0;
    p->packet_size = read_varint(buffer, &num_read);
    p->packet_id = read_varint(buffer, &num_read);
    p->packet_data = (char*) malloc(p->packet_size);
    memcpy(p->packet_data, buffer + num_read, p->packet_size);
    return p;
}

void free_packet(packet* p) {
    free(p->packet_data);
    free(p);
}

void free_handshake_packet(handshake_packet* hp) {
    free(hp->server_address);
    free(hp);
}

void print_packet(packet* p) {
    printf("Packet Size: %d\n", p->packet_size);
    printf("Packet ID: %d\n", p->packet_id);
    printf("Packet Contents: \n");
    for (int i = 0; i < p->packet_size; i++) {
        printf("%x ", p->packet_data[i]);
    }
    printf("\n");
    fflush(stdout);
}

void print_handshake_packet(handshake_packet* hp) {
    printf("Protocol Version: %d\n", hp->protocol_version);
    printf("Server Address: %s\n", hp->server_address);
    printf("Server Port: %d\n", hp->server_port);
    printf("Next State: %d\n", hp->next_state);
    fflush(stdout);
}