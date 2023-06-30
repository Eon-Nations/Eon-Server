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
    return value;
}

uint16_t read_ushort(uint8_t *data, uint32_t *bytes_read) {
    uint16_t value = 0;
    value |= data[0] << 8;
    value |= data[1];
    *bytes_read += 2;
    return value;
}

packet_t* read_packet(uint8_t* buffer, uint32_t buffer_size, uint32_t* bytes_read) {
    packet_t* packet = malloc(sizeof(packet_t));
    uint32_t packet_length_read = 0;
    int16_t packet_length = read_varint(buffer, buffer_size, &packet_length_read);
    if (packet_length == 0) {
        packet->size = 0;
        packet->data = NULL;
        packet->packet_id = 0x0;
        *bytes_read += packet_length_read;
        return packet;
    }
    uint32_t packet_id_len = 0;
    packet->packet_id = read_varint(buffer + packet_length_read, buffer_size, &packet_id_len);
    packet_length_read += packet_id_len;
    packet->size = packet_length - packet_id_len;
    packet->data = malloc(packet_length);
    memcpy(packet->data, buffer + packet_length_read, packet_length);
    *bytes_read += packet_length_read + packet_length;
    return packet;
}

void read_packets(packet_list_t* packets, uint8_t* buffer, uint32_t buffer_size) {
    uint32_t bytes_read = 0;
    uint32_t packets_read = 0;
    do {
        packet_t* packet = read_packet(buffer + bytes_read, buffer_size, &bytes_read);
        bytes_read -= 1;
        packets->packets[packets_read] = packet;
        packets_read++;
    } while (bytes_read < buffer_size && packets_read < MAX_PACKETS_READ);
    packets->size = packets_read;
}

void print_packet(packet_t* packet) {
    printf("Packet ID: %d\n", packet->packet_id);
    printf("Packet Size: %d\n", packet->size);
    printf("Packet Data: ");
    for (uint32_t i = 0; i < packet->size; i++) {
        printf("%02x ", packet->data[i]);
    }
    printf("\n");
}

void print_packets(packet_list_t* packets) {
    printf("Printing %d packets:\n", packets->size);
    for (uint32_t i = 0; i < packets->size; i++) {
        printf("Packet %d:\n", i);
        print_packet(packets->packets[i]);
    }
}

void free_packet(packet_t* packet) {
    free(packet->data);
    free(packet);
}

void free_stack_packet_list(packet_list_t* packets) {
    for (uint32_t i = 0; i < packets->size; i++) {
        free_packet(packets->packets[i]);
    }
    free(packets->packets);
}

void free_packet_list(packet_list_t* packets) {
    free_stack_packet_list(packets);
    free(packets);
}