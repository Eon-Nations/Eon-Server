#define HANDSHAKE_PACKET_ID 0x0;
#define LOGIN_START_PACKET_ID 0x0;

#define PING_REQUEST_PACKET_ID 0x1;
#define PING_RESPONSE_PACKET_ID 0x1;

typedef struct {
    int protocol_version;
    char* server_address;
    unsigned short server_port;
    int next_state;
} handshake_packet;

typedef struct {
    char* username;
    char has_uuid;
    char* uuid;
} login_start_packet;

