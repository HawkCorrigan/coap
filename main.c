#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "udpListener.h"
#include "udpSender.h"
#include "coap_message.h"

/*
* Global variable message 
*/
coap_message_t *message;

void createmsg(char* content, size_t size);
void memread(char* buf);

int main() 
{
	char* msgbuf;

	/*
	* save the recieved UDP data into msgbuf
	*/

    if(startListener(msgbuf) != 0) {
        printf("udpListener failed to bind");
        return -1;
    }

	coap_message_t *message = malloc(sizeof(coap_message_t));
    if(message == NULL) {
        printf("Memory allocation failed");
        return -1;
    }

	/*
	* parse the raw UDP data, and process it
	*/

	memread(msgbuf);

	size_t size = 4;

	/*
	* turn message back into bitstring to send via UDP
	*/

	createmsg(msgbuf, size);
	startSender(msgbuf, size);

	free(message);
}

void memread(char* buf)
{
	int msglen = sizeof(buf);
	parse(message, (uint8_t*) buf, msglen);
	printf("proto vers: %d\n", message->header->vers);
	printf("mess type: %d\n", message->header->type);
	printf("tkn len: %d\n", message->header->token_len);
	printf("msg code: %d.%d\n", message->header->code_type, message->header->code_status);
	int i;
	for(i=0;i<message->numopts;i++){
		printf("option: %d.%02x\n", message->opts[i].number,*message->opts[i].value.p);
	}
	printf("msg id: %d\n", message->header->message_id);


	if (message->payload.len != 0) {
		printf("payload: %s\n", message->payload);
	} 
	else {
		printf("no payload available\n");
	}

	printf("\n");
}

void createmsg(char* content, size_t size)
{
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
