#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>

#include "coapMessage.h"
#include "errors.h"

#define BUF_SIZE 512

/*
 * memread is placeholder until application context is given, to read buf received
 */
 
void memread(char* buf, int count){
	

    coap_message_t *message = malloc(sizeof(coap_message_t));

	if (NULL == message) {
		printf("Error allocating memory for message");
		return;
	}

	int error = parse(message, (uint8_t*) buf, count);
	
	if (error != SUCCESS) {
		switch (error) {
			case ERROR_WRONG_VERSION:
				return;
			case ERROR_MESSAGE_FORMAT:
				printf("Recieved message was formatted wrongly.");
				return;
			case ERROR_WRONG_CONTEXT: //This error cannot have happend at this point.
				printf("Recieved message had wrong/unknown context");
				return;
			case ERROR_UNRECOGNISED_OPTION: //This error cannot have happend at this point.
			    printf("Recieved message had an unknown option");
				return;
		}
	}

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
		printf("payload: %s\n", message->payload.p);
	} else {
		printf("no payload available\n");
	}

	printf("\n");
}

int startListener(char* buf, size_t *msg_size)
{
	/*
	 * one of host or port can be NULL, if unknown
	 */
	 
	char* host = NULL;
	char* port = "5683";
	
	struct addrinfo hints;
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE|AI_ADDRCONFIG;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	
	struct addrinfo *result, *tmp;
	int fd, sockfd;

	fd = getaddrinfo(host, port, &hints, &result);
	if(fd != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(fd));
		exit(ERROR_UDP_LISTENER_ADDR_INFO);
	}
	
	/*
	 * iterate over all addrinfo received by getaddrinfo until end of linked list
	 * or until a socket could be bound
	 */
	 
	for (tmp = result; tmp != NULL; tmp = tmp->ai_next) {
		
		sockfd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (sockfd == -1) {
			fprintf(stderr, "Socket: %i not found! \n %s \n", sockfd, strerror(errno));
			continue;
		}
		
		if (bind(sockfd, tmp->ai_addr, tmp->ai_addrlen) != 0) {
			fprintf(stderr, "Error with Bind: %s \n",strerror(errno));
			return -1;
		}
		else {
			printf("Binding with %i \n", sockfd);
			break;
		}
		
		close(sockfd);
	}	

	freeaddrinfo(result);
	
	/*
	 * keep reading Datagrams, no msg received until application context is given
	 */
	 
	for(;;) {

		struct sockaddr_storage peer_addr;
		socklen_t peer_addr_len = sizeof(peer_addr);

		ssize_t count = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);

		if (count == -1) {
			continue;
		} else {
			printf("received %zu bytes of data \n", count);
			*msg_size=(size_t)count;
			return 0;
		}

	}

}
