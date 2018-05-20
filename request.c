#include <stdio.h>
#include <stdint.h>
#include "request.h"

/*
*  IDEA: save currently pending requests. When a new request is made to the server,
*        save all of its necessary data to a struct:
   A CoAP request consists of the method to be applied to the resource,
   the identifier of the resource, a payload and Internet media type (if
   any), and optional metadata about the request.
*
*       handlereq is one part of request-response matching, the others being handleres and callmatch
*/

int createreq(coap_message_t* msg, coap_request_t* req)
{
/* 
*   check cody type to be 0.xx
*/
    if(msg->header->code_type != 0){
        printf("invalid request code!\n");
        return -1;  // TODO: Respond with 4.05 Method not allowed
    }
/* 
*   set endpoint path and port to the destination as set in URI path and URI port optoins
*/
    req->endpoint->method = msg->header->code_status;
    for(int i = 0; i < msg->numopts; i++){
        if(msg->opts->number == 7){
            req->endpoint->path->number = msg->opts->value.p; // david pls help me here and make it work
        }
    }

    for(int i = 0; i < msg->numopts; i++){
        if(msg->opts->number == 11){
            req->endpoint->path->dest = msg->opts->value.p; // why? p is a pointer, dest is a pointer!?
        }
    }
/* 
*   TODO: set identifier of the resource to be interacted with
*/

/* 
*   set accept option
*/
    for(int i = 0; i < msg->numopts; i++){
        if(msg->opts->number == 17){
            req->mediatype = msg->opts->value.p;
        }
    }

/* 
*   set payload
*/
    req->payload = msg->payload;

/* 
*   set token
*/
    req->token = msg->token.p;

    return 0;
}