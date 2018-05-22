#pragma once
enum ERRORS {
    //Recieved Message FORMAT Errors
    ERROR_WRONG_VERSION = 11,
    ERROR_MESSAGE_FORMAT = 12,
    //Recieved Message CONTENT Errors
    ERROR_WRONG_CONTEXT = 21,
    ERROR_UNRECOGNISED_OPTION = 22,

    ERROR_UDP_SENDER_ADDR_INFO = 81,
    ERROR_UDP_LISTENER_ADDR_INFO = 86,

    // Message allocation error 
    ERROR_MALLOC_MESSAGE_HEADER = 91,
    ERROR_MALLOC_MESSAGE_OPTS = 92,
    ERROR_MALLOC_MESSAGE_PAYLOAD = 93,

    SUCCESS = 0
};