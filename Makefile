CFLAGS = -g -Wall -Wpedantic
COBJ = clientMain.o udpSender.o udpListener.o coapMessage.o idgen.o
SOBJ = serverMain.o udpListener.o udpSender.o coapMessage.o idgen.o
CC = gcc

all: coap-client coap-server

%.o: %.c
	$(CC) $(CFLAGS) -c $<

coap-client : $(COBJ)
	$(CC) $(CFLAGS) -o coap-client $(COBJ)

coap-server : $(SOBJ)
	$(CC) $(CFLAGS) -o coap-server $(SOBJ)

