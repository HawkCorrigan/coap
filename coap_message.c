#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "coap_message.h"

int parseHeader(coap_header_t *header, uint8_t *bitstring) { //1=Ignore, 0=Success, -1=Error

    header->vers = ((bitstring[0] & 0b11000000)>>6);

    if (header->vers!=1)
        return 1;

    header->type = ((bitstring[0] & 0b00110000) >> 4);
    header->token_len = ((bitstring[0] & 0b00001111));

    if (header->token_len > 8) {
        return -1;
    }

    header->code_type = ((bitstring[1] & 0b11100000) >> 5);
    header->code_status = ((bitstring[1] & 0b00011111));

    header->message_id = ((bitstring[2] << 8) | bitstring[3]);

    return 0;
}

int parse(coap_message_t *message, uint8_t *bitstring, int udp_message_len) {
    int readpos = 0;
    message->header=malloc(sizeof(coap_header_t));
    int success = parseHeader(message->header, bitstring);

    message->token = 0;
    readpos = 4;

    for (int i = 0; i < message->header->token_len; i++) {
        message->token = ((message->token) << 8) | bitstring[4 + i];
        readpos++;
    }

    message->opts = malloc(sizeof(coap_option_t));

    int optCount = 0;
    uint16_t rollingDelta=0;
    while (bitstring[readpos] != 0xFF && readpos < udp_message_len) {
        optCount++;
        message->opts = realloc(message->opts, (size_t) (optCount * sizeof(coap_option_t)));

        uint8_t cbyte = bitstring[readpos];

        uint8_t delta = (cbyte & 0xF0) >> 4;
        uint8_t o_len = (cbyte & 0x0F);

        if (delta == 15 || o_len == 15) {
            exit(1);
        }
        if (delta == 13) {
            delta=bitstring[++readpos]+13;
        }

        if (delta == 14) {
            delta = (bitstring[++readpos] << 8) | bitstring[++readpos]+269;
        }

        if (o_len == 13) {
            o_len = bitstring[++readpos]+13;
        }

        if (o_len == 14) {
            o_len = (bitstring[++readpos] << 8) | bitstring[++readpos]+269;
        }

        message->opts[optCount-1].number=delta+rollingDelta;
        message->opts[optCount-1].value.len=o_len;
        message->opts[optCount - 1].value.p = &(bitstring[++readpos]);
        
        readpos+=o_len;
    }

    message->numopts=optCount;

    if (readpos == udp_message_len) {
        message->payload.len = 0;
        return 0;
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

    return 0;
}