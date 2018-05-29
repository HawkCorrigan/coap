#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "coapMessage.h"
#include "errors.h"

int initEmptyMessage(coap_message_t *msg) {
    msg->header = malloc(sizeof(coap_header_t));
    msg->header->vers = 1;
    msg->header->type = 0;
    msg->header->token_len = 0;
    msg->header->code_type = 0;
    msg->header->code_status = 0;
    msg->header->message_id = 0;

    msg->token.len = 0;
    msg->token.p = NULL;

    msg->numopts = 0;
    msg->opts = NULL;
    
    msg->payload.len = 0;
    msg->payload.p = NULL;
}

int parseHeader(coap_header_t *header, uint8_t *bitstring) {

    header->vers = ((bitstring[0] & 0b11000000)>>6);

    if (header->vers!=1)
        return ERROR_WRONG_VERSION; //Ignore Message

    header->type = ((bitstring[0] & 0b00110000) >> 4);
    header->token_len = ((bitstring[0] & 0b00001111));

    if (header->token_len > 8) {
        return ERROR_MESSAGE_FORMAT;
    }

    header->code_type = ((bitstring[1] & 0b11100000) >> 5);
    header->code_status = ((bitstring[1] & 0b00011111));

    header->message_id = ((bitstring[2] << 8) | bitstring[3]);

    return SUCCESS;
}

int parse(coap_message_t *message, uint8_t *bitstring, int udp_message_len) {
    int readpos = 0;
    message->header = malloc(sizeof(coap_header_t));

    if (NULL == message->header) {
        return ERROR_MALLOC_MESSAGE_HEADER;
    }

    int error = parseHeader(message->header, bitstring);
    
    if (SUCCESS != error) {
        return error;
    }

    readpos = 4;
    

    if(message->header->token_len==0){
        message->token.len = 0;
        message->token.p = NULL;
    }

    if(message->header->token_len<8){
        message->token.p=bitstring+4;
        message->token.len=message->header->token_len;
    }
    readpos += message->header->token_len;

    message->opts = malloc(sizeof(coap_option_t));

    if (NULL == message->opts) {
        return ERROR_MALLOC_MESSAGE_OPTS;
    }

    int optCount = 0;
    uint16_t rollingDelta=0;
    while (bitstring[readpos] != 0xFF && readpos < udp_message_len) {
        optCount++;
        message->opts = realloc(message->opts, (size_t) (optCount * sizeof(coap_option_t)));

        if (NULL == message->opts) {
            return ERROR_MALLOC_MESSAGE_OPTS;
        }

        uint8_t cbyte = bitstring[readpos];

        uint8_t delta = (cbyte & 0xF0) >> 4;
        uint8_t o_len = (cbyte & 0x0F);

        if (delta == 15 || o_len == 15) {
            return ERROR_MESSAGE_FORMAT;
        }
        if (delta == 13) {
            delta=bitstring[++readpos]+13;
        }

        if (delta == 14) {
            delta = (bitstring[++readpos] << 8) | bitstring[++readpos]+269;
        }

        if (o_len == 13) {
            o_len = bitstring[++readpos]+13;
        }

        if (o_len == 14) {
            o_len = (bitstring[++readpos] << 8) | bitstring[++readpos]+269;
        }

        message->opts[optCount-1].number = delta+rollingDelta;
        message->opts[optCount-1].value.len = o_len;
        message->opts[optCount - 1].value.p = &(bitstring[++readpos]);
        
        readpos += o_len;
    }

    message->numopts=optCount;

    if (readpos == udp_message_len) {
        message->payload.len = 0;
        return SUCCESS;
    }

    if (bitstring[readpos - 1] == 0xFF) {
        return ERROR_MESSAGE_FORMAT;
    }

    readpos++;
    int payload_byte_count = udp_message_len - readpos;

    message->payload.len = (size_t) udp_message_len - readpos + 1;

    message->payload.p = malloc((payload_byte_count + 1) * sizeof(uint8_t));

    if (NULL == message->payload.p) {
        return ERROR_MALLOC_MESSAGE_PAYLOAD;
    }

    memcpy(message->payload.p, bitstring + readpos, (size_t) payload_byte_count);

    message->payload.p[payload_byte_count] = '\0';

    return SUCCESS;
}


int build(uint8_t *buf, size_t *buflen, const coap_message_t *msg){

    buf = realloc(buf, 4 * sizeof(uint8_t));
    buf[0] = (msg->header->vers & 0x03) << 6;
    buf[0] |= (msg->header->type & 0x03 ) << 4;
    buf[0] |= (msg->header->token_len&0x0F);
    buf[1] = msg->header->code_type << 5;
    buf[1] |= msg->header->code_status;
    buf[2] = msg->header->message_id >> 8;
    buf[3] = msg->header->message_id;

    *buflen = 4;
    uint8_t *p = buf + 4;

    if (msg->header->token_len > 0){
        *buflen += msg->header->token_len;
        buf = realloc(buf, ((uint8_t) *buflen) * sizeof(uint8_t));
        if (buf == NULL)
            return ERROR_MALLOC_MESSAGE_TOKEN;
        memcpy(p, msg->token.p, msg->header->token_len);
    }

    p += msg->header->token_len;

    uint8_t runningDelta = 0;
    for(int  i = 0; i < msg->numopts; i++)
    {
        int optDelta = msg->opts[i].number - runningDelta;
        uint8_t length, delta;
        
        if (optDelta < 13){
            delta= 0xFF & optDelta;
        }
        else if (optDelta <= 13+0xFF){
            delta= 13;
        }
        else if (optDelta <= 269+0xFFFF){
            delta = 14;
        }
        
        (*buflen)++;

        int optLength = msg->opts[i].value.len;
        if (optLength < 13){
            length= 0xFF & optLength;
        }
        else if (optLength <= 13+0xFF){
            length = 13;
        }
        else if (optLength <= 269+0xFFFF){
            length = 14;
        }
        *p++ = delta<<4 | length;
        if(delta==13){
            *p++=optDelta-13;
            (*buflen)++;
        }
        if(delta==14){
            *p++ = (optDelta - 269) >> 8;
            *p++ = (0xFF & (optDelta - 269));
            (*buflen) += 2;
        }

        if(length==13){
            *p++ = msg->opts[i].value.len -13;
            (*buflen)++;
        }
        if(length==14){
            *p++ = (msg->opts[i].value.len-269)>>8;
            *p++ = (0xFF & (msg->opts[i].value.len-269));
            (*buflen) += 2;
        }

        buf = realloc(buf, ((uint8_t) *buflen) * sizeof(uint8_t));
        if (buf == NULL)
            return ERROR_MALLOC_MESSAGE_TOKEN;

        memcpy(p, msg->opts[i].value.p, msg->opts[i].value.len);
        p += msg->opts[i].value.len;
        runningDelta = msg->opts[i].number;
    }

    if (msg->payload.len > 0){
        *buflen += 1 + msg->payload.len;
        buf = realloc(buf, ((uint8_t) *buflen) * sizeof(uint8_t));
        if (buf == NULL)
            return ERROR_MALLOC_MESSAGE_TOKEN;

        *p++=0xFF;
        memcpy(p, msg->payload.p, msg->payload.len);
    }

    return SUCCESS;
}
