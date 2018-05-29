CFLAGS = -g -Wall -Wpedantic
COBJ = clientMain.c udpSender.o udpListener.o coapMessage.o
SOBJ = coapServer.c udpListener.o udpSender.o coapMessage.o
CC = /usr/bin/gcc

all: coap-client coap-server

coap-client : $(CBJ)
	$(CC) $(CFLAGS) -o coap-client $(COBJ)

coap-server : $(SOBJ)
	$(CC) $(CFLAGS) -o coap-server $(SOBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<