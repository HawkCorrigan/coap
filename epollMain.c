#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "time.h"
#include "coapMessage.h"
#include "endpoint.h"

#define MAXEVENTS 64
#define ERROR -1
#define SUCCESS 0
#define COAP_PORT "5683"

coap_endpoint_t endpoints[150];
coap_coms_buffer_t outgoingMessages;

static int make_socket_non_blocking (int sfd) {
    int flags, s;

    flags = fcntl (sfd, F_GETFL, 0);
    if (flags == -1) {
        perror ("fcntl");
        return ERROR;
    }

    flags |= O_NONBLOCK;
    s = fcntl (sfd, F_SETFL, flags);
    if (s == -1) {
        perror ("fcntl");
        return ERROR;
    }

    return 0;
}

static int create_and_bind (char *port) {
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo (NULL, port, &hints, &result);
    if (s != 0) {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return ERROR;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == ERROR)
        continue;

        s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == SUCCESS) {
            break;
        }
        close (sfd);
    }

    if (rp == NULL) {
        fprintf (stderr, "Could not bind\n");
        return ERROR;
    }

    freeaddrinfo (result);
    return sfd;
}

int main (int argc, char *argv[]) {
    int i=0;
    for (i=0;i<150;i++){
        endpoints[i].coap_endpoint_function=NULL;
        endpoints[i].ct=NULL;
        endpoints[i].path=NULL;
        endpoints[i].method=(coap_method_t)0;
    }
    int sfd, s;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    outgoingMessages.stor = malloc(sizeof(coap_out_msg_storage_t));
    outgoingMessages.length = 0;
    outgoingMessages.capacity=1;
    outgoingMessages.stor[0].msg=NULL;    

    sfd = create_and_bind (COAP_PORT);
    if (sfd == ERROR){
        abort ();
    }

    s = make_socket_non_blocking (sfd);
    if (s == ERROR){
        abort ();
    }

    efd = epoll_create1 (0);
    if (efd == ERROR){
        perror ("epoll_create");
        abort ();
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
    if (s == -1){
        perror ("epoll_ctl");
        abort ();
    }
    events = calloc (MAXEVENTS, sizeof event);

    while (1){
        int n, i;

        n = epoll_wait (efd, events, MAXEVENTS, 100);
        for (i = 0; i < n; i++){
	        if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))){
	            fprintf (stderr, "epoll error\n");
	            close (events[i].data.fd);
	            continue;
	        } else {
                while (1){
                    printf("STARTRECV\n");
                    ssize_t count;
                    char buf[512];
                    struct sockaddr_storage peer_addr;
                    socklen_t peer_addr_len = sizeof(peer_addr);
                    count = recvfrom (events[i].data.fd, buf, sizeof buf, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
                    printf("DONERECV\ns");
                    if (count == -1) {
                        if (errno != EAGAIN) {
                            perror ("read");
                        }
                    break;
                    } else if (count == 0){
                        break;
                    }
                    handleIncomingMessage(buf,(size_t)count);
                    if (s == -1) {
                        perror ("write");
                        abort ();
                    }
                }
            }
        }
        int index;
        coap_message_t *outMsg = malloc(sizeof(coap_message_t));
        initEmptyMessage(outMsg);
        
        if ((index=getNextMessage(outMsg))!=-1){
            dumpMessage(outMsg);
            ssize_t count;
            size_t *msg_size = malloc (sizeof(size_t));
            uint8_t *buf=malloc(sizeof(uint8_t));
            printf("STARTBUILD\n");
            dumpMessage(outMsg);
            build(buf, msg_size, outMsg);
            printf("TRYING TO SEND %zu Bytes: %s\n", *msg_size, buf);
            struct sockaddr_storage peer_addr;
            socklen_t peer_addr_len = sizeof(peer_addr);
            count = sendto(events[i].data.fd, buf, 6, 0, (struct sockaddr *) &peer_addr, peer_addr_len);
            printf("%zd\n", count);
            if (count == -1){
                return -1;
            }
        }
    }

    free (events);
    close (sfd);
    return EXIT_SUCCESS;
}