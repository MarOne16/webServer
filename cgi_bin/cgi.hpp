#pragma once

#include "../Response/webserver.hpp"

class http_items;

typedef struct envirmoment
{
    std::string  SERVER_SOFTWARE;
    std::string  REQUEST_METHOD;
    std::string  QUERY_STRING;
    std::string  CONTENT_TYPE;
    std::string  CONTENT_LENGTH;
    std::string  SCRIPT_FILENAME;
    std::string  SCRIPT_NAME;
    std::string  REQUEST_URI;
    std::string  DOCUMENT_ROOT;
    std::string  DOCUMENT_URI;
    std::string  SERVER_PROTOCOL;
    std::string  SERVER_ADDR;
    std::string  SERVER_PORT;
    std::string  SERVER_NAME;
    static const size_t NUM_ELEMENTS = 14;
} envirmoment;

typedef struct cgi_data
{
    envirmoment env_server;
    std::string body;
}   cgi_data;

cgi_data GET_CGI_DATA(http_items &response_items);
envirmoment GET_SETENV_SERVER(http_items &response_items);
void ADD_TOENV_SERVER(envirmoment &env_server);
void FREEENV_SERVER(envirmoment &env_server);
std::string FINAL_RESPONSE(http_items &response_items);
void debud_cgi_data(cgi_data &cgi);

#include "../Response/webserver.hpp"