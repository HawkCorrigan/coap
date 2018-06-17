#include "endpoint.h"

void generate_wk_core(){
    int i=0;
    const coap_endpoint_t *ep = endpoints;
    while (endpoints->coap_endpoint_function!=NULL){
            if (strlen(wk_core)>0){
                strncat(wk_core, ",",1);
            }
            strncat(wk_core, "<",1);

            for (i=0;i<ep->path->length;i++){
                strncat(wk_core,"/",1);
                strncat(wk_core,ep->path->dest[i],WK_CORE_LENGTH);
            }

            strncat(wk_core, ">;", 2);
            strncat(wk_core, ep->ct,WK_CORE_LENGTH);
            ep++;
    }
}