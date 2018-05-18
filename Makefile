#CoAP_Server : main.o coap_message.o udpListener.o udpSender.o
#		gcc -std=gnu99 -Wall -o CoAp_Server main.o coap_message.o udpListener.o udpSender.o

CoAP_Server : main.c udpListener.h coap_message.h udpSender.h udpListener.c udpSender.c coap_message.c 
		gcc -std=gnu99 -Wall -o main.o main.c coap_message.h udpListener.h udpSender.h udpSender.c udpListener.c coap_message.c

#udpListner.o : udpListener.c udpListener.h
#		gcc -std=gnu99 -Wall -o udpListener.o udpListner.c udpListener.h

#udpSender.o : udpSender.c udpSender.h
#		gcc -std=gnu99 -Wall -o udpSender.o udpSender.c udpSender.h

#coap_message.o : coap_message.c coap_message.h
#		gcc -std=gnu99 -Wall -o coap_message.o coap_message.c coap_message.h
