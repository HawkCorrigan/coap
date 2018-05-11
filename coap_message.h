#include <stdio.h>
#include <malloc.h>

typedef struct {
    uint8_t vers;
    uint8_t type;
    uint8_t token_len;
    uint8_t code_type;
    uint8_t code_status;
    uint16_t message_id;
} coap_header_t;

typedef struct {
    uint8_t *p;
    size_t len;
} coap_buffer_t;

typedef struct {
    uint16_t delta;
    coap_buffer_t value;
} coap_option_t;

typedef struct {
    coap_header_t *header;
    uint64_t token;
    uint8_t numopts;
    coap_option_t *opts;
    coap_buffer_t payload;
} coap_message_t;

coap_header_t *parseHeader(uint8_t *bitstring) {
    coap_header_t *header = malloc(sizeof(coap_header_t));

    if ((bitstring[0] & 0b11000000) != 0b01000000) {
        //TODO ignore message silently
    } else {
        header->vers = 1;
    }

    header->type = (unsigned) ((bitstring[0] & 0b00110000) >> 4);
    header->token_len = (unsigned) ((bitstring[0] & 0b00001111));

    if (header->token_len > 8) {
        //TODO Message format error
    }

    header->code_type = (unsigned) ((bitstring[1] & 0b11100000) >> 5);
    header->code_status = (unsigned) ((bitstring[1] & 0b00011111));

    header->message_id = (unsigned) ((bitstring[2] << 8) | bitstring[3]);

    return header;
}

coap_message_t parse(uint8_t *bitstring, int udp_message_len) {
    int readpos = 0;
    coap_message_t *message = malloc(sizeof(coap_message_t));

    message->header = parseHeader(bitstring);

    message->token = 0;
    readpos = 4;

    for (int i = 0; i < message->header->token_len; i++) {
        message->token = ((message->token) << 8) | bitstring[4 + i];
        readpos++;
    }

    message->opts = malloc(sizeof(coap_option_t));

    int optCount = 0;
    while (bitstring[readpos] != 0xFF && readpos < udp_message_len) {
        optCount++;
        message->opts = realloc(message->opts, (size_t) (optCount * sizeof(coap_option_t)));

        uint8_t cbyte = bitstring[readpos];

        message->opts[optCount - 1].delta = (cbyte & 0b11110000) >> 4;
        uint8_t delta = message->opts[optCount - 1].delta;
        message->opts[optCount - 1].value.len=(size_t)(cbyte & 0b00001111);
        uint8_t o_len = message->opts[optCount-1].value.len;

        if (delta == 15 || o_len == 15) {
            exit(1);
        }
        if (delta == 13 || delta == 14) {
            message->opts[optCount - 1].delta = bitstring[++readpos];
        }

        if (delta == 14) {
            message->opts[optCount - 1].delta =
                    (message->opts[optCount - 1].delta << 8) | bitstring[++readpos];
        }

        if (o_len == 13 || o_len == 14) {
            message->opts[optCount - 1].value.len = bitstring[++readpos];
        }

        if (o_len == 14) {
            message->opts[optCount - 1].value.len =
                    (message->opts[optCount - 1].value.len << 8) | bitstring[++readpos];
        }

        message->opts[optCount - 1].value.p =
                malloc(message->opts[optCount - 1].value.len * sizeof(uint8_t));
        for (int i = 0; i < message->opts[optCount - 1].value.len; i++) {
            message->opts[optCount - 1].value.p[i] = bitstring[++readpos];
        }

        readpos++;
    }

    message->numopts=optCount;

    if (readpos == udp_message_len) {
        message->payload.len = 0;
        return *message;
    }

    if (bitstring[readpos - 1] == 0xFF) {
        //TODO Message format error
    }

    readpos++;
    int payload_byte_count = udp_message_len - readpos;

    message->payload.len = (size_t) udp_message_len - readpos + 1;

    message->payload.p = malloc((payload_byte_count + 1) * sizeof(uint8_t));

    memcpy(message->payload.p, bitstring + readpos, (size_t) payload_byte_count);

    message->payload.p[payload_byte_count] = '\0';

    return *message;
}
