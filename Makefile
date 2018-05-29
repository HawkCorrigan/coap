CFLAGS = -g -Wall
OBJ = clientMain.c udpSender.o coapMessage.o
CC = /usr/bin/gcc
 
coapClient : $(OBJ)
	$(CC) $(CFLAGS) -o coapServer $(OBJ)
 
%.o: %.c
	$(CC) $(CFLAGS) -c $<