#pragma once
enum ERRORS {
    //Recieved Message FORMAT Errors
    ERROR_WRONG_VERSION = 11,
    ERROR_MESSAGE_FORMAT = 12,
    //Recieved Message CONTENT Errors
    ERROR_WRONG_CONTEXT = 21,
    ERROR_UNRECOGNISED_OPTION = 22,

    SUCCESS = 0
};