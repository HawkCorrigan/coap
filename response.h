#include <stdio.h>
#include <stdint.h>
#include "request.h"

int handleres(coap_message_t* inmsg, coap_clientrequest_t* req);
int makeres(coap_message_t* msg, coap_request_t* req);