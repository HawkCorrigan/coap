#include <stdio.h>
#include <malloc.h>

typedef struct {
    coap_header_t header;
    coap_buffer_t token;
    uint8_t numopts;
    coap_option_t opts[MAXOPTS];
    char *payload;
} Message;

typedef struct {
    uint8_t vers;
    uint8_t token;
    uint8_t token_len;
    uint8_t code_type;
    uint8_t code_status;
    uint8_t message_id;
} coap_header_t;

typedef struct {
    uint8_t *p;
    size_t len;
} coap_buffer_t;

typedef struct {
    uint8_t *p;
    size_t len;
} coap_buffer_t;

typedef struct {
    uint8_t num;
    coap_buffer_t buf;
} coap_option_t;

int main() {
    return 0;
}

struct Message parse(char *bitstring, int udp_message_len) {
    int readpos = 0;
    struct Message *message = malloc(sizeof(struct Message));
    if ((bitstring[0] & 0b11000000) != 0b01000000) {
        //TODO ignore message silently
    }
    message->m_type = (unsigned) ((bitstring[0] & 0b00110000) >> 4);
    message->token_len = (unsigned) ((bitstring[0] & 0b00001111));
    if (message->token_len > 8) {
        //TODO Message format error
    }
    readpos = 1;
    message->code_type = (unsigned) ((bitstring[1] & 0b11100000) >> 5);
    message->code_status = (unsigned) ((bitstring[1] & 0b00011111));
    readpos = 2;
    message->message_id = (unsigned) ((bitstring[2] << 8) | bitstring[3]);
    message->token = 0;
    readpos = 4;

    for (int i = 0; i < message->token_len; i++) {
        message->token = ((message->token) << 8) | bitstring[4 + i];
        readpos++;
    }

    while (bitstring[readpos] != 0xFF && readpos < udp_message_len) {
        // TODO parse options
        readpos++;
    }

    if (readpos == udp_message_len)
        return *message;

    if (bitstring[readpos - 1] == 0xFF) {
        //TODO Message format error
    }

    readpos++;
    int payload_byte_count = 0;
    message->payload = malloc(sizeof(char));
    for (readpos; readpos < udp_message_len; readpos++) {
        payload_byte_count++;
        message->payload = realloc(message->payload, (size_t) payload_byte_count);
        message->payload[payload_byte_count - 1] = bitstring[readpos];
    }
}
