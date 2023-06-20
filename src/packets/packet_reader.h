#include "../varint/varint.h"
#include "../test.h"
#include "status_packet.h"
#include <assert.h>

typedef struct {
    int packet_size;
    int packet_id;
    char *packet_data;
} packet;

handshake_packet* read_hp(packet* p);
packet* read_packet(char* buffer);

void free_packet(packet* p);
void free_handshake_packet(handshake_packet* hp);

void print_packet(packet* p);
void print_handshake_packet(handshake_packet* hp);