#include "endpoint.h"
#include <string.h>

void generate_wk_core(){
    wk_core[0]=0;
    int i=0;
    const coap_endpoint_t *ep = endpoints;
    while (ep->coap_endpoint_function!=NULL){
            if (strlen(wk_core)>0){
                strcat(wk_core, ",");
            }
            strcat(wk_core, "<");
            for (i=0;i<ep->path->length;i++){
                strcat(wk_core,"/");
                strcat(wk_core,ep->path->dest[i]);
            }

            strcat(wk_core, ">;");
            ep++;
    }
}
static const coap_endpoint_path_t wk_path={2, {".well-known", "core"}};
static const coap_endpoint_path_t hello_world_path={2, {"hello", "world"}};
static const coap_endpoint_path_t hello_echo_path={2, {"hello", "echo"}};
void initializeEndpoints(){
    int i;
    for (i=0;i<150;i++){
        endpoints[i].coap_endpoint_function=NULL;
        endpoints[i].ct=NULL;
        endpoints[i].path=NULL;
        endpoints[i].method=(coap_method_t)0;
    }
    endpoints[0].ct=NULL;
    endpoints[0].path=&wk_path;
    endpoints[0].method=COAP_METHOD_GET;
    endpoints[0].coap_endpoint_function=wk_core_handler;

    endpoints[1].ct=NULL;
    endpoints[1].path=&hello_world_path;
    endpoints[1].method=COAP_METHOD_GET;
    endpoints[1].coap_endpoint_function=hello_world_handler;

    endpoints[2].ct=NULL;
    endpoints[2].path=&hello_echo_path;
    endpoints[2].method=COAP_METHOD_POST;
    endpoints[2].coap_endpoint_function=hello_echo_handler;
}

int wk_core_handler(const coap_message_t *inmsg, coap_message_t *out){
    generate_wk_core();
    return makeResponse(out, (uint8_t *)wk_core, strlen(wk_core), inmsg->header->message_id, &inmsg->token, 2,5);
}

int hello_world_handler(const coap_message_t *inmsg, coap_message_t *out){
    printf("HELLO WORLD\n");
    return makeResponse(out, NULL, 0, inmsg->header->message_id, &inmsg->token, 2,0); 
}

int hello_echo_handler(const coap_message_t *inmsg, coap_message_t *out){
    printf("JODELING NOISES\n");
    return makeResponse(out, inmsg->payload.p, inmsg->payload.len, inmsg->header->message_id, &inmsg->token, 2,5);
}