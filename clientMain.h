#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "coapMessage.h"
#include "udpSender.h"
#include "idgen.h"


void usage(const char *program);
void addOption(coap_message_t *msg, char *optnum, char *optvalue);
int main(int argc, char const *argv[]);