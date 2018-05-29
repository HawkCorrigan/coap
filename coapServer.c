#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "udpListener.h"
#include "udpSender.h"
#include "coapMessage.h"

void createExampleMsg(char* content, size_t size);
void memread(char* buf, coap_message_t *msg);

int main() 
{
	char* msgbuf = malloc(512*sizeof(char));
    size_t *msg_size=malloc(sizeof(size_t));

	/*
	* save the received UDP data into msgbuf
	*/

    if(startListener(msgbuf) != 0) {
        printf("udpListener failed to bind");
        return -1;
    }

	coap_message_t *msg = malloc(sizeof(coap_message_t));
    if(msg == NULL) {
        printf("Memory allocation failed");
        return -1;
    }

	/*
	* parse the raw UDP data, and process it
	*/

	parse(msg, msgbuf, *msg_size);
}

void buildExampleMsg(char* content, size_t size)
{

	coap_message_t *message;

    message->header=malloc(sizeof(coap_header_t));
    message->header->vers=1;
    message->header->type=0;
    message->header->token_len=0;
    message->header->code_type=1;
    message->header->message_id=29546;
    message->token.p=NULL;
    message->token.len=0;
    message->numopts=0;
    message->payload.len=0;
    message->payload.p=NULL;

    build(content,size,message);
}
