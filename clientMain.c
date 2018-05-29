#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "coapMessage.h"
#include "udpSender.h"


void usage(const char *program)
{
    const char *p;

    p = strrchr(program, '/');
    if (p)
        program = ++p;

    fprintf(stderr,
            "usage: %s [OPTIONS] URI\n\n"
            "\tURI can be an absolute or relative coap URI,\n"
            //"\t-a addr\tthe local interface address to use\n"
            "\t-e text\t\tinclude text as payload (use percent-encoding for\n"
            "\t\t\tnon-ASCII characters)\n"
            "\t-f file\t\tfile to send with PUT/POST (use '-' for STDIN)\n"
            "\t-h host\t\thostname or ip of remote\n"
            "\t-m method\trequest method (get|put|post|delete), default is 'get'\n"
            "\t-N\t\tsend NON-confirmable message\n"
            "\t-o num,text\tadd option num with contents text to request\n"
            //"\t-O file\t\toutput received data to this file (use '-' for STDOUT)\n"
            "\t-p port\n"
            "\t-T token\tinclude specified token\n",
            program);
}

void addOption(coap_message_t *msg, char *optnum, char *optvalue) {
    msg->numopts++;
    msg->opts = realloc(msg->opts, msg->numopts * sizeof(coap_option_t));
    msg->opts[msg->numopts - 1].number = (uint8_t)atoi(optnum);
    msg->opts[msg->numopts - 1].value.len = strlen(optvalue);
    msg->opts[msg->numopts - 1].value.p = (uint8_t *) optvalue;
}
int main(int argc, char const *argv[])
{
    printf("Me!");
    if (argc == 1)
    {
        usage(argv[0]);
        exit(1);
    }

    const char *host = "localhost";
    const char *port = "5683";
    coap_message_t *message = malloc(sizeof(coap_message_t));
    message->header = malloc(sizeof(coap_header_t));
    int opt = 0;

    extern char* optarg;

    initEmptyMessage(message);

    while ((opt = getopt(argc, (char* const*)argv, "Ne:f:h:m:o:p:O:T:")) != -1)
    {
        switch (opt)
        {
        case 'e':
            message->payload.len = strlen(optarg);
            message->payload.p = malloc(message->payload.len * sizeof(char));
            memcpy(message->payload.p, optarg, message->payload.len);
            break;
        case 'f':
            //TODO
            break;
        case 'h':
            host = optarg;
            break;
        case 'm':
            message->header->code_type = MESSAGE_TYPE_REQUEST;
            if (!strcmp(optarg, "get") || !strcmp(optarg, "GET")) {
                message->header->code_status = REQUEST_TYPE_GET;
            } else
            if (!strcmp(optarg, "post") || !strcmp(optarg, "POST")) {
                message->header->code_status = REQUEST_TYPE_POST;
            } else
            if (!strcmp(optarg, "put") || !strcmp(optarg, "PUT")) {
                message->header->code_status = REQUEST_TYPE_PUT;
            } else
            if (!strcmp(optarg, "delete") || !strcmp(optarg, "DELETE")) {
                message->header->code_status = REQUEST_TYPE_DELETE;
            } else {
                printf("Unknown method %s.", optarg);
                usage(argv[0]);
            }
            break;
        case 'o':
            addOption(message, strtok(optarg, ",:="), strtok(NULL, ",:="));
            break;
        case 'O':
            //TODO
            break;
        case 'p':
            port = optarg;
            break;
        case 'T':
            message->token.len = sizeof(optarg);
            message->token.p = malloc(message->token.len);
            memcpy(message->token.p, optarg, message->token.len);
            break;
        case 'N':
            message->header->type = 1;
            break;
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    uint8_t *package = malloc(sizeof(uint8_t));
    size_t *buflen = malloc(sizeof(size_t));
    build(package, buflen, message);
    startSender(host, port, (char*)package, buflen);
    return 0;
}
