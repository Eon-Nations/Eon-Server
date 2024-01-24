#include "clientbound_packets.h"
#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

status_response_t* create_status_response() {
    status_response_t* status_response = malloc(sizeof(status_response_t));
    FILE* file = fopen("src/test.json", "r");
    if (file == NULL) {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        printf("Opening the file has gone wrong. Here's why: %s\n", strerror(errno));
        printf("Current working dir: %s\n", cwd);
        fflush(stdout);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    status_response->json_response_length = file_size;
    rewind(file);

    status_response->json_response = malloc(file_size + 1);
    fread(status_response->json_response, file_size, 1, file);
    fclose(file);

    status_response->json_response[file_size] = '\0';
    return status_response;
}

void print_status_response(status_response_t* packet) {
    printf("Status Response Packet:\n");
    printf("JSON Response Length: %ld\n", packet->json_response_length);
    printf("JSON Response: %s\n", packet->json_response);
}

void write_status_response(status_response_t* packet, uint8_t* buffer, uint32_t* bytes_written) {
    uint32_t varint_length = 0;
    write_varint(buffer, (int32_t) packet->json_response_length, &varint_length);
    memcpy(buffer + *bytes_written, packet->json_response, packet->json_response_length);
    *bytes_written += packet->json_response_length + varint_length;
}

void free_status_response(status_response_t* packet) {
    free(packet->json_response);
    free(packet);
}
