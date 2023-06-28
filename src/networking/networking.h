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

packet_t* read_packet(uint8_t* buffer, uint32_t buffer_size, uint32_t* bytes_read);

void read_packets(packet_list_t* packets, uint8_t* buffer, uint32_t buffer_size);

void print_packet(packet_t* packet);
void print_packets(packet_list_t* packets);
void free_packet(packet_t* packet);

void free_stack_packet_list(packet_list_t* packets);
void free_packet_list(packet_list_t* packets);

#endif