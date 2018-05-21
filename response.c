#include <stdio.h>
#include <stdlib.h>
#include "request.h"

/* 
*   handlres is the CLIENT-SIDE function for resolving an incoming response. it compares the response to the currently
*   active request, returns -1 if it doesn't match, and returns 0 if it does.
*/
int handleres(coap_message_t* inmsg, coap_clientrequest_t* req)
{
    uint8_t msgtoken = inmsg->token.p;
    uint8_t reqtoken = req->token;

    if(msgtoken == reqtoken){
        return 0;
    }
    else return -1;
}

/*
*    makeres is the SERVER-SIDE function to set the token and endpointdata of the currently active request
*   to the message that is currently being sent. req should be freed after this.
*/
int makeres(coap_message_t* msg, coap_request_t* req)
{

/* 
*   set endpoint path and port to the destination as set in URI path and URI port optoins
*/
    for(int i = 0; i < msg->numopts; i++){
        if(msg->opts[i].number == 7){
            msg->opts[i].value.p = req->endpoint->path->number;
        }
    }

    for(int i = 0; i < msg->numopts; i++){
        if(msg->opts[i].number == 11){
            msg->opts[i].value.p = req->endpoint->path->dest;
        }
    }

/* 
*   set token
*/
    msg->token.p = req->token;
}
