CFLAGS = -g -Wall
OBJ = coapServer.o udpListener.o udpSender.o coap_message.o
CC = gcc

coapServer : $(OBJ)
			$(CC) $(CFLAGS) -o coapServer $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<