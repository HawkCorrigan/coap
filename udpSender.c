#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

#include "coapMessage.h"


void startSender(){
    const char* host="8.8.8.8"; /* localhost */
    const char* port="5683";
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_protocol=0;
    hints.ai_flags=AI_ADDRCONFIG;
    struct addrinfo* res=0;
    int err=getaddrinfo(host,port,&hints,&res);
    if (err!=0) {
        exit(1);
    }
    
    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(fd==-1){
        exit(1);
    }


    size_t *size = malloc(sizeof(size_t)); 
    char *package = malloc(sizeof(char)); 
    coap_message_t *msg = malloc(sizeof(coap_message_t)); 
    msg->header=malloc(sizeof(coap_header_t));
    msg->header->vers=1;
    msg->header->type=0;
    msg->header->token_len=1;
    msg->header->code_type=1;
    msg->header->message_id=29546;
    msg->token.len=1;
    msg->token.p=malloc(1 * sizeof(uint8_t));
    msg->token.p[0]=5;
    msg->numopts=0;
    msg->payload.len=0;
    msg->payload.p=NULL;
    build(package, size, msg);

    printf("Sender: %d\n", *size);
    printf("%d\n", *package);
    if (sendto(fd, package, *size, 0, res->ai_addr,res->ai_addrlen) ==-1) {
        exit(1);
    }
}