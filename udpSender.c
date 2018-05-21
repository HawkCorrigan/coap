#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

void startSender(char* buf, size_t size){
    const char* host=0; /* localhost */
    const char* port="1234";
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

    if (sendto(fd,buf,size,0,
    res->ai_addr,res->ai_addrlen)==-1) {
        exit(1);
    }

}