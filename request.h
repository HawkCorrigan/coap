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

/* 
*   the client does not need to save its own endpoint data, nor does it need to know
*   what payload it sent. It only needs to know what media type it should expect and
*   what token to match it to.
*/
typedef struct {
    uint8_t *mediatype;     //from accept option
    uint8_t token;
} coap_clientrequest_t;

int handlereq(coap_message_t* msg, coap_request_t* req);
int makereq(coap_message_t* outmsg, coap_clientrequest_t* req);