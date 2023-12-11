#pragma once

// #include "../Response/webserver.hpp"
#include <iostream>
#include <map>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include <dirent.h>
#include <fstream>

class http_items;


// typedef struct cgi_data
// {
//     std::string body;
//     unsigned int status_code;
// }

typedef struct envirmoment
{
    // std::string HTTP_COOKIE;
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
    std::string  PATH_INFO;
    std::string  PATH_TRANSLATED;
    std::string  REDIRECT_STATUS;
    std::string  REDIRECT_STATUS_;
    static const size_t NUM_ELEMENTS = 17;
} envirmoment;

typedef struct cgi_data
{
    http_items &response_tools;
    envirmoment env_server;
    std::string body;
    std::string cgi_response;
    std::string status_code;
    std::string status_message;
    std::map<std::string, std::string> cgi_headers;

    // Constructor with initializer list to initialize the reference member
    cgi_data(http_items &rt, const envirmoment &es, const std::string &b)
        : response_tools(rt), env_server(es), body(b) {}
} cgi_data;

std::string EXEC_CGI(cgi_data &cgi, char **extra_env);
cgi_data GET_CGI_DATA(http_items &response_items);
envirmoment GET_SETENV_SERVER(http_items &response_items);
char **GET_EXTRA_ENV(envirmoment &env_server);
void FREEENV_SERVER(envirmoment &env_server);
cgi_data &FINAL_RESPONSE(http_items &response_items);
void debud_cgi_data(cgi_data &cgi);

#include "../Response/webserver.hpp"