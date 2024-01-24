#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    uint8_t *data;
    uint8_t packet_id;
    uint32_t size;
} packet_t;

typedef struct {
    packet_t** packets;
    uint32_t size;
} packet_list_t;

int32_t read_varint(uint8_t *data, uint32_t size, uint32_t *bytes_read);
void write_varint(uint8_t* buffer, int32_t value, uint32_t *bytes_written);

uint16_t read_ushort(uint8_t *data, uint32_t *bytes_read);

packet_t* read_packet(uint8_t* buffer, uint32_t buffer_size, uint32_t* bytes_read);
void write_packet(packet_t* packet, uint8_t* buffer, uint32_t* bytes_written);
void read_packets(packet_list_t* packets, uint8_t* buffer, uint32_t buffer_size);


void print_packet(packet_t* packet);
void print_packets(packet_list_t* packets);
void free_packet(packet_t* packet);

void free_stack_packet_list(packet_list_t* packets);
void free_packet_list(packet_list_t* packets);

#endif
