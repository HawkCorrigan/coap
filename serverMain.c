#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "udpListener.h"
#include "udpSender.h"
#include "coapMessage.h"
#include "idgen.h"

void createExampleMsg(char* content, size_t size);
void memread(char* buf, coap_message_t *msg);

int main() 
{
    initializeRNG();
	char* msgbuf = malloc(512*sizeof(char));
    size_t *msg_size=malloc(sizeof(size_t));

    if(startListener(msgbuf, msg_size) != 0) {
        printf("udpListener failed to bind");
        return -1;
    }

	coap_message_t *msg = malloc(sizeof(coap_message_t));
    if(msg == NULL) {
        printf("Memory allocation failed");
        return -1;
    }

	parse(msg, (uint8_t*)msgbuf, *msg_size);
    dumpMessage(msg);
}
