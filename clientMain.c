#include <stdint.h>
#include <unistd.h>

#include "coapMessage.h"
#include "udpSender.h"

int main(int argc, char const *argv[])
{
    if (argc == 1)
    {
        usage(argv[0]);
        exit(1);
    }

    const char *host = "localhost";
    const char *port = "5683";
    const char *payload = NULL;
    coap_message_t *message = malloc(sizeof(coap_message_t));
    message->header = malloc(sizeof(coap_header_t));
    int opt;

    initEmptyMessage(message);

    while ((opt = getopt(argc, argv, "Ne:f:h:m:o:p:O:T:")) != -1)
    {
        switch (opt)
        {
        case 'e':
            message->payload.len = sizeof(optarg);
            message->payload.p = malloc(message->payload.len);
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
            switch (optarg) {
                case 'get':
                case 'GET':
                message->header->code_status = 1;
                break;
                case 'post':
                case 'POST':
                message->header->code_status = 1;
                break;
                case 'put':
                case 'PUT':
                message->header->code_status = 1;
                break;
                case 'delete':
                case 'DELETE':
                message->header->code_status = 1;
                break;
                default:
                printf("Unknown method %s.", optarg);
                usage(argv[0]);
            }
            break;
        case 'o':
            //TODO
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
    startSender(host, port, package, buflen);
    return 0;
}

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
            "\t-h host\n"
            "\t-m method\trequest method (get|put|post|delete|fetch|patch|ipatch), default is 'get'\n"
            "\t-N\t\tsend NON-confirmable message\n"
            "\t-o file\t\toutput received data to this file (use '-' for STDOUT)\n"
            "\t-p port\n"
            "\t-O num,text\tadd option num with contents text to request\n"
            "\t-T token\tinclude specified token\n",
            program);
}