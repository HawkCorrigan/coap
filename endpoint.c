#include "endpoint.h"
#include <string.h>

void generate_wk_core(){
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

    generate_wk_core();
}

static int wk_core_handler(const coap_message_t *inmsg, coap_message_t *out){
    return makeResponse(out, (uint8_t *)wk_core, strlen(wk_core), inmsg->header->message_id, &inmsg->token, 2,5);
}