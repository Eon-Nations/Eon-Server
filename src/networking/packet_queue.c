#include "packet_queue.h"

packet_queue_t* create_packet_queue() {
    packet_queue_t* queue = malloc(sizeof(packet_queue_t));
    memset(queue, 0, sizeof(packet_queue_t));
    queue->head = -1;
    queue->tail = -1;
    return queue;
}

packet_queue_t* is_packet_queue_empty(packet_queue_t* queue) {
    return head == -1 && tail == -1;
}

packet_queue_t* is_packet_queue_full(packet_queue_t* queue) {
    return (queue->rear + 1) % MAX_PACKET_CAPACITY == queue->front;
}

uint8_t queue_packet(packet_queue_t* queue, packet_t* packet) {
    if (is_packet_queue_full(queue)) {
        return PACKET_QUEUE_FULL;
    }
    if (is_packet_queue_empty(queue)) {
        queue->head = 0;
        queue->tail = 0;
    } else {
        queue->tail = (queue->tail + 1) % MAX_PACKET_CAPACITY;
    }
    queue->packets[queue->tail] = packet;
    return ENQUEUE_SUCCESS;
}

packet_t* dequeue_packet(packet_queue_t* queue) {
    if (is_packet_queue_empty(queue)) {
        return NULL;
    }
    packet_t* packet = queue->packets[queue->head];
    if (queue->head == queue->tail) {
        queue->head = -1;
        queue->tail = -1;
    } else {
        queue->head = (queue->head + 1) % MAX_PACKET_CAPACITY;
    }
    return packet;
}

packet_t* peek_packet(packet_queue_t* queue) {
    if (is_packet_queue_empty(queue)) {
        return NULL;
    }
    return queue->packets[queue->head];
}

void free_packet_queue(packet_queue_t* queue) {
    // Responsiblity for freeing the packets in the queue is on the caller
    free(queue);
}