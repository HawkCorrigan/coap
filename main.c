#include <stdio.h>
#include "udpListener.h"
#include "udpSender.h"
#include "coap_message.h"

/*
* Global variable message 
*/
coap_message_t *message;

int main() 
{
	/*
	* save the recieved UDP data into msgbuf
	*/

    char* msgbuf = startListener();
    if(msgbuf == NULL) {
        printf("udpListener failed to bind");
        return;
    }

	coap_message_t *message = malloc(sizeof(coap_message_t));
    if(message == NULL) {
        printf("Memory allocation failed");
        return;
    }

	/*
	* parse the raw UDP data, and process it
	*/

	memread(msgbuf);

	size_t size = 4;

	/*
	* turn message back into bitstring to send via UDP
	*/

	char* msgbuf = createmsg(size);
	startSender(msgbuf);

	free(message);
}

coap_message_t* memread(char* buf)
{
	int msglen = sizeof(buf);
	int success = parse(message, (uint8_t*) buf, msglen);
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

	return message;
}

char* createmsg(size_t size)
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

	char* content;
    build(content,size,message);

	return content;
}