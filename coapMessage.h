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

int initEmptyMessage(coap_message_t *);
int parseHeader(coap_header_t *header, uint8_t *bitstring);
int parse(coap_message_t *message, uint8_t *bitstring, int udp_message_len);
