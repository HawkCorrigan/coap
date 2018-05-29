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

enum {
    MESSAGE_TYPE_REQUEST = 0,
    MESSAGE_TYPE_RESPONSE_SUCCESS = 2,
    MESSAGE_TYPE_RESPONSE_ECLIENT = 4,
    MESSAGE_TYPE_RESPONSE_ESERVER = 5
};

enum {
    REQUEST_TYPE_GET = 1,
    REQUEST_TYPE_POST,
    REQUEST_TYPE_PUT,
    REQUEST_TYPE_DELETE
};

int initEmptyMessage(coap_message_t *);
int parseHeader(coap_header_t *header, uint8_t *bitstring);
int parse(coap_message_t *message, uint8_t *bitstring, size_t msg_size);
int build(uint8_t *buf, size_t *buflen, const coap_message_t *msg);
int dumpMessage(coap_message_t *msg);