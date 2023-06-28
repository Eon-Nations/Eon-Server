#include "networking.h"

#define SEGMENT_BITS 0x7F
#define SEGMENT_CONTINUATION 0x80
#define MAX_PACKETS_READ 50

int32_t read_varint(uint8_t *data, uint32_t data_size, uint32_t *bytes_read) {
    int32_t value = 0;
    uint32_t position = 0;
    int8_t current_byte = 0;
    do {
        current_byte = data[position];
        value |= (current_byte & SEGMENT_BITS) << (7 * position);
        position++;
    } while ((current_byte & SEGMENT_CONTINUATION) != 0 && position < data_size);
    *bytes_read = position;
    printf("Bytes read: %d\n", position);
    fflush(stdout);
    return value;
}

packet_t* read_packet(uint8_t* buffer, uint32_t buffer_size, uint32_t* bytes_read) {
    packet_t* packet = malloc(sizeof(packet_t));
    uint32_t packet_length_read = 0;
    int16_t packet_length = read_varint(buffer, buffer_size, &packet_length_read);
    printf("Packet length: %d\n", packet_length);
    uint8_t* packet_data = malloc(packet_length);
    memcpy(packet_data, buffer + packet_length_read, packet_length);
    packet->data = packet_data;
    for (int16_t i = 0; i < packet_length; i++) {
        packet->data[i] = packet_data[i];
    }
    *bytes_read = packet_length_read + packet_length;
    return packet;
}

packet_list_t* read_packets(uint8_t* buffer, uint32_t buffer_size) {
    uint32_t bytes_read = 0;
    uint32_t packets_read = 0;
    packet_list_t* packets = malloc(sizeof(packet_list_t));
    packets->packets = malloc(sizeof(packet_t*) * MAX_PACKETS_READ);
    packets->size = 0;
    while (bytes_read < buffer_size && packets_read < MAX_PACKETS_READ) {
        packet_t* packet = read_packet(buffer + bytes_read, buffer_size - bytes_read, &bytes_read);
        packets->packets[packets_read] = packet;
        packets->size++;
        packets_read++;
    }
    return packets;
}

void print_packet(packet_t* packet) {
    printf("Packet ID: %d\n", packet->packet_id);
    printf("Packet Size: %d\n", packet->size);
    printf("\n");
}

void print_packets(packet_list_t* packets) {
    printf("Printing %d packets:\n", packets->size);
    for (uint32_t i = 0; i < packets->size; i++) {
        print_packet(packets->packets[i]);
    }
}

void free_packet(packet_t* packet) {
    free(packet->data);
    free(packet);
}

void free_packet_list(packet_list_t* packets) {
    for (uint32_t i = 0; i < packets->size; i++) {
        free(packets->packets[i]);
    }
    free(packets->packets);
    free(packets);
}