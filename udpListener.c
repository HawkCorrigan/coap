#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>

#include "coap_message.h"

#define BUF_SIZE 512

/*
 * memread is placeholder until application context is given, to read buf received
 */
 
void memread(char* buf, int count){
	
	coap_message_t message = parse((uint8_t*) buf, count);
	printf("proto vers: %d\n", message.header->vers);
	printf("mess type: %d\n", message.header->type);
	printf("tkn len: %d\n", message.header->token_len);
	printf("msg code: %d.%d\n", message.header->code_type, message.header->code_status);
	int i;
	for(i=0;i<message.numopts;i++){
		printf("option: %d.%02x\n", message.opts[i].number,*message.opts[i].value.p);
	}
	printf("msg id: %d\n", message.header->message_id);



	if (message.payload.len != 0) {
		printf("payload: %s\n", message.payload);
	} else {
		printf("no payload available\n");
	}

	printf("\n");
}

void startListener()
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
		exit(EXIT_FAILURE);
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
			exit(EXIT_FAILURE);
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

		char buf[BUF_SIZE];
		struct sockaddr_storage peer_addr;
		socklen_t peer_addr_len = sizeof(peer_addr);

		ssize_t count = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);

		if (count == -1) {
			continue;
		} else {
			printf("received %zu bytes of data \n", count);
			memread(buf, count);
		}

	}
}
