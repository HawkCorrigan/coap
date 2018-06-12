#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "time.h"
#include "endpoint.h"

#include "coapMessage.h"
#include "errors.h"

extern coap_endpoint_t endpoints[];

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
    
    return 0;
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

int parse(coap_message_t *message, uint8_t *bitstring, size_t msg_size) {
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
    while (bitstring[readpos] != 0xFF && readpos < msg_size) {
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
            delta=(bitstring[++readpos]+13);
        }

        if (delta == 14) {
            delta = bitstring[++readpos] << 8;
            delta |= bitstring[++readpos]+269;
        }

        if (o_len == 13) {
            o_len = (bitstring[++readpos]+13);
        }

        if (o_len == 14) {
            o_len = bitstring[++readpos] << 8; 
            o_len |= bitstring[++readpos]+269;
        }

        message->opts[optCount-1].number = delta+rollingDelta;
        message->opts[optCount-1].value.len = o_len;
        message->opts[optCount - 1].value.p = &(bitstring[++readpos]);
        
        readpos += o_len;
    }

    message->numopts=optCount;

    if (readpos == msg_size) {
        message->payload.len = 0;
        return SUCCESS;
    }

    if (bitstring[readpos - 1] == 0xFF) {
        return ERROR_MESSAGE_FORMAT;
    }

    readpos++;
    int payload_byte_count = msg_size - readpos;

    message->payload.len = msg_size - readpos + 1;

    message->payload.p = malloc((payload_byte_count + 1) * sizeof(uint8_t));

    if (NULL == message->payload.p) {
        return ERROR_MALLOC_MESSAGE_PAYLOAD;
    }

    memcpy(message->payload.p, bitstring + readpos, (size_t) payload_byte_count);

    message->payload.p[payload_byte_count] = '\0';

    return SUCCESS;
}

int dumpMessage(coap_message_t *msg){
    printf("proto vers: %d\n", msg->header->vers);
	printf("mess type: %d\n", msg->header->type);
	printf("tkn len: %d\n", msg->header->token_len);
	printf("msg code: %d.%d\n", msg->header->code_type, msg->header->code_status);
	int i;
	for(i=0;i<msg->numopts;i++){
		printf("option: %d.%02x\n", msg->opts[i].number,*msg->opts[i].value.p);
	}
	printf("msg id: %d\n", msg->header->message_id);


	if (msg->payload.len != 0) {
		printf("payload: %s\n", msg->payload.p);
	} 
	else {
		printf("no payload available\n");
	}

	printf("\n");

    return 0;
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

int handleIncomingMessage(char* buf, size_t length){ //return index of response in outgoingmessages or -1 on error
    int s = 0;
    coap_message_t *msg = malloc(sizeof(coap_message_t));
    s = parse(msg, buf, length);
    coap_out_msg_storage_t coms;
    coap_message_t *resp = malloc(sizeof(coap_message_t));
    if ((s = getResponse(msg, resp))!=-1){

        add_acknowledge(msg->header->message_id);
        time_t now = time(NULL);
        coms.failedattempts=0;
        coms.lasttransmission=0;
        coms.msg=resp;
        coms.recvtime=now;
        int i;
        if(outgoingMessages.capacity==outgoingMessages.length){
            outgoingMessages.stor=realloc(outgoingMessages.stor, outgoingMessages.capacity*2);
            if(outgoingMessages.stor==NULL)
                return -1;
            outgoingMessages.capacity*=2;
        }
        while ((!(outgoingMessages.stor[i].msg==NULL))&&(i<outgoingMessages.length)){
            i++;
        }
            outgoingMessages.stor[i]=coms;
        return i;
     } else {
         addReset(msg->header->message_id);
     }
}

int addReset(uint16_t mid){
    coap_message_t *rsp;
    rsp=initEmptyMessage;
    rsp->header->type=RST;
    rsp->header->message_id=mid;
    coap_out_msg_storage_t coms;
    time_t now = time(NULL);
    coms.failedattempts=0;
    coms.lasttransmission=0;
    coms.msg=rsp;
    coms.recvtime=now;
    int i;
    if (outgoingMessages.capacity==outgoingMessages.length){
        outgoingMessages.stor = realloc(outgoingMessages.stor, outgoingMessages.capacity*2);
        if(outgoingMessages.stor==NULL)
            return -1;
        outgoingMessages.capacity*=2;
    }
    while((!(outgoingMessages.stor[i].msg==NULL))&&(i<outgoingMessages.length)){
        i++;
    }
    outgoingMessages.stor[i]=coms;
}

int add_acknowledge(uint16_t mid){
    coap_message_t *rsp;
    rsp=initEmptyMessage;
    rsp->header->type=ACK;
    rsp->header->message_id=mid;
    coap_out_msg_storage_t coms;
    time_t now = time(NULL);
    coms.failedattempts=0;
    coms.lasttransmission=0;
    coms.msg=rsp;
    coms.recvtime=now;
    int i;
    if (outgoingMessages.capacity==outgoingMessages.length){
        outgoingMessages.stor = realloc(outgoingMessages.stor, outgoingMessages.capacity*2);
        if(outgoingMessages.stor==NULL)
            return -1;
        outgoingMessages.capacity*=2;
    }
    while((!(outgoingMessages.stor[i].msg==NULL))&&(i<outgoingMessages.length)){
        i++;
    }
    outgoingMessages.stor[i]=coms;
}
    
int getResponse(const coap_message_t *in, coap_message_t *out){
    int i=0;
    const coap_endpoint_t *ep = endpoints;
    while(NULL != endpoints->coap_endpoint_function){
        return endpoints->coap_endpoint_function(in, out->header->message_id, out->payload.p, out->payload.len, out->header->code_status, out->header->code);
    }
    build_response(out, NULL, 0, in->header->message_id, in->token, COAP_RESPONSE_NOT_FOUND, COAP_CONTENTTYPE_NONE);
    return 0;
}

int delayMessage(int i){
    outgoingMessages.stor[i].lasttransmission=now();
    outgoingMessages.stor[i].failedattempts++;
}