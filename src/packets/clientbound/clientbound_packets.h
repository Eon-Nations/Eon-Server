#ifndef CLIENTBOUND_PACKETS_H
#define CLIENTBOUND_PACKETS_H

#include <stdint.h>
#include <string.h>
#include "../../networking/networking.h"

typedef struct {
    int64_t json_response_length;
    char* json_response;
} status_response_t;

status_response_t* create_status_response();
void write_status_response(status_response_t* packet, uint8_t* buffer, uint32_t* bytes_written);
void print_status_response(status_response_t* packet);
void free_status_response(status_response_t* packet);

typedef struct {
    int64_t timestamp;
} ping_response_t;

ping_response_t create_ping_response();

#endif
