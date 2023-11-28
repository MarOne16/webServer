#pragma once
#include "../Response/webserver.hpp"

char **_ENV_SERVER;


typedef struct envirmoment
{
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
    static const size_t NUM_ELEMENTS = 13;
} envirmoment;

typedef struct cgi_data
{
    envirmoment env_server;
    std::string body;
    std::string path;
    std::string method;
    std::string query;
    std::string content_type;
    std::string content_length;
    std::string script_filename;
    std::string script_name;
    std::string request_uri;
    std::string document_root;
    std::string document_uri;
    std::string server_protocol;
    std::string server_addr;
}   cgi_data;

cgi_data GET_CGI_DATA(std::string &request);
envirmoment GET_ENV_SERVER(std::string &request);
void ADD_TO_ENV_SERVER(envirmoment &env_server, char **env);
void FREE_ENV_SERVER(envirmoment &env_server);
std::string FINAL_RESPONSE();