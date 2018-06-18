#pragma once
#include <stdio.h>
#include <stdint.h>
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
    uint16_t number;
    coap_buffer_t value;
} coap_option_t;

typedef struct {
    coap_header_t *header;
    coap_buffer_t token;
    uint8_t numopts;
    coap_option_t *opts;
    coap_buffer_t payload;
} coap_message_t;

typedef struct coap_out_msg_storage_t{
    coap_message_t *msg;
    uint64_t recvtime;
    uint64_t lasttransmission;
    uint8_t failedattempts;
} coap_out_msg_storage_t;

typedef struct coap_coms_buffer_t {
    coap_out_msg_storage_t *stor;
    size_t length;
    size_t capacity;
} coap_coms_buffer_t;

enum {
    MESSAGE_TYPE_REQUEST = 0,
    MESSAGE_TYPE_RESPONSE_SUCCESS = 2,
    MESSAGE_TYPE_RESPONSE_ECLIENT = 4,
    MESSAGE_TYPE_RESPONSE_ESERVER = 5
};

enum {
    REQUEST_TYPE_GET = 1,
    REQUEST_TYPE_POST = 2,
    REQUEST_TYPE_PUT = 3,
    REQUEST_TYPE_DELETE = 4,
};

enum {
    NON = 1,
    CON = 2,
    ACK = 3,
    RST = 4,
};

int initEmptyMessage(coap_message_t *);
int parseHeader(coap_header_t *header, uint8_t *bitstring);
int parse(coap_message_t *message, uint8_t *bitstring, size_t msg_size);
int build(uint8_t *buf, size_t *buflen, const coap_message_t *msg);
int dumpMessage(coap_message_t *msg);
int handleIncomingMessage(char *msg, size_t length);
int addReset(uint16_t mid);
int add_acknowledge(uint16_t mid);
int getResponse(const coap_message_t *in, coap_message_t *out);
int addToOutgoing(coap_out_msg_storage_t coms);
int makeResponse(coap_message_t *msg, const uint8_t *content, size_t content_length, uint16_t mid,const coap_buffer_t *tok, uint8_t c_stat, uint8_t c_type);
const coap_option_t *getOption(const coap_message_t *msg, uint8_t num, uint8_t *count);
int getNextMessage(coap_message_t *out);

extern coap_coms_buffer_t outgoingMessages;