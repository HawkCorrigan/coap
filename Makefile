main.o : main.c udpListener.h coap_message.h udpSender.h udpListener.c udpSender.c coap_message.c 
		gcc -std=gnu99 -Wall -o main.o main.c coap_message.h udpListener.h udpSender.h udpSender.c udpListener.c coap_message.c

