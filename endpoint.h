#include "coapMessage.h"

typedef enum
{
    COAP_METHOD_GET = 1,
    COAP_METHOD_POST = 2,
    COAP_METHOD_PUT = 3,
    COAP_METHOD_DELETE = 4
} coap_method_t;

typedef struct {
    int number;
    const char *dest;
} coap_endpoint_path_t;

typedef struct {
    coap_method_t method;
    char *(*outCharBuffer)(coap_message_t inMessage);
    const coap_endpoint_path_t *path;
    const char* ct;
} coap_endpoint_t;


