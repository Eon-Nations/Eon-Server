#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#include "networking.h"

#define MAX_PACKET_CAPACITY 1096
#define PACKET_QUEUE_FULL 0x1
#define PACKET_QUEUE_EMPTY 0x2
#define ENQUEUE_SUCCESS 0x3

typedef struct {
    packet_t* packets[MAX_PACKET_CAPACITY];
    int32_t head;
    int32_t tail;
} packet_queue_t;

packet_queue_t* create_packet_queue();
packet_queue_t* is_packet_queue_empty(packet_queue_t* queue);
packet_queue_t* is_packet_queue_full(packet_queue_t* queue);
uint8_t queue_packet(packet_queue_t* queue, packet_t* packet);
packet_t* dequeue_packet(packet_queue_t* queue);
packet_t* peek_packet(packet_queue_t* queue);

void free_packet_queue(packet_queue_t* queue);

#endif