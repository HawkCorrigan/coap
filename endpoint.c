#include "endpoint.h"
#include <string.h>

void generate_wk_core(){
    int i=0;
    coap_endpoint_t endpoints[150];
    const coap_endpoint_t *ep = endpoints;
    while (endpoints->coap_endpoint_function!=NULL){
            if (strlen(wk_core)>0){
                strcat(wk_core, ",");
            }
            strcat(wk_core, "<");

            for (i=0;i<ep->path->length;i++){
                strcat(wk_core,"/");
                strcat(wk_core,ep->path->dest[i]);
            }

            strcat(wk_core, ">;");
            strcat(wk_core, ep->ct);
            ep++;
    }
}