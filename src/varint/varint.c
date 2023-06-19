#include "varint.h"
#include <stdio.h>

uint16_t read_ushort(const char* buffer) {
    uint16_t value = (buffer[0] << 8) | buffer[1];
    return value;
}

int read_varint(char* buffer, int* num_read) {
    int result = 0;
    int shift = 0;
    int times_in_loop = 0;
    char read;
    do {
        read = buffer[*num_read];
        result |= (read & 0x7F) << shift;
        shift += 7;
        (*num_read)++;
        times_in_loop++;
    } while ((read & 0x80) != 0);
    return result;
}

