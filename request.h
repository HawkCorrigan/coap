#include <stdio.h>
#include <stdint.h>
#include "endpoint.h"
#include "coap_message.h"
/* 
*   NOTE: endpoint.h and request.h could be merged into one, especially since
*         endpoint is/should not be used outside of request/response scope
*/
typedef struct {
    coap_endpoint_t *endpoint;
    coap_buffer_t payload;  //message payload
    uint8_t *mediatype;     //from accept option
    uint8_t token;
} coap_request_t;

int createreq(coap_message_t* msg, coap_request_t* req)