CFLAGS = -g -Wall -Wpedantic
COBJ = clientMain.o udpSender.o udpListener.o coapMessage.o idgen.o
EOBJ = epollMain.o udpListener.o udpSender.o coapMessage.o idgen.o endpoint.o
CC = gcc

all: coap-client epoll-main

%.o: %.c
	$(CC) $(CFLAGS) -c $<

coap-client : $(COBJ)
	$(CC) $(CFLAGS) -o coap-client $(COBJ)

epoll-main : $(EOBJ)
	$(CC) $(CFLAGS) -o epoll-main $(EOBJ)
