#include "coapMessage.h"

typedef enum
{
    COAP_METHOD_GET = 1,
    COAP_METHOD_POST = 2,
    COAP_METHOD_PUT = 3,
    COAP_METHOD_DELETE = 4
} coap_method_t;

typedef enum
{
    COAP_SUCCESS=2,
    COAP_CLIENTERROR=4,
    COAP_SERVERERROR=5,
} coap_code_type_t;

typedef enum
{
    COAP_SUCCESS_CREATED=1,
    COAP_SUCCESS_DELETED=2,
    COAP_SUCCESS_VALID=3,
    COAP_SUCCESS_CHANGED=4,
    COAP_SUCCESS_CONTENT=5,
} coap_success_status_t;

typedef enum
{
    COAP_CLIENTERROR_UNAUTHORIZED = 1,
    COAP_CLIENTERROR_BADOPTION = 2,
    COAP_CLIENTERROR_FORBIDDEN = 3,
    COAP_CLIENTERROR_NOTFOUND = 4,
    COAP_CLIENTERROR_METHODNOTALLOWED = 5,
    COAP_CLIENTERROR_NOTACCEPTABLE = 6,
    COAP_CLIENTERROR_PRECONDITIONFAILED = 12,
    COAP_CLIENTERROR_REQUESTENTITYTOOLARGE = 13,
    COAP_CLIENTERROR_UNSUPPORTEDCONTENTFORMAT = 14,
} coap_clienterror_status_t;

typedef enum
{
    COAP_SERVERERROR_INTERNALSERVERERROR = 0,
    COAP_SERVERERROR_NOTIMPLEMENTED = 1,
    COAP_SERVERERROR_BADGATEWAY = 2,
    COAP_SERVERERROR_SERVICEUNAVAILABLE = 3,
    COAP_SERVERERROR_GATEWAYTIMEOUT = 4,
    COAP_SERVERERROR_PROXYINGNOTSUPPORTED = 5,
} coap_servererror_status_t;

typedef struct {
    int length;
    const char *dest[4];
} coap_endpoint_path_t;

typedef struct {
    coap_method_t method;
    int (*coap_endpoint_function)(const coap_message_t *inmsg, uint16_t mid, char *content, size_t content_length, uint8_t code);
    const coap_endpoint_path_t *path;
    const char* ct;
} coap_endpoint_t;

const uint16_t wk_core_length = 1500;
static char wk_core[wk_core_length] = "";
void generate_wk_core();

coap_endpoint_t endpoints[];

void generate_wk_core(){
    int i=0;
    const coap_endpoint_t *ep = endpoints;
    while (endpoints->coap_endpoint_function!=NULL){
            if (strlen(wk_core)>0){}
                strncat(wk_core, ",",1);
            }
            strncat(wk_core, "<",1);

            for (i=0;i<ep->path->length;i++){
                strncat(wk_core,"/",1);
                strncat(wk_core,ep->path->dest[i],wk_core_length);
            }

            strncat(wk_core, ">;", 2);
            strncat(wk_core, ep->ct,wk_core_length);
            ep++;
    }
}