CFLAGS = -g -Wall
OBJ = udpListener.o udpSender.o coapMessage.o
CC = /usr/bin/gcc
 
coapServer : $(OBJ)
	$(CC) $(CFLAGS) -o coapServer $(OBJ)
 
%.o: %.c
	$(CC) $(CFLAGS) -c $<