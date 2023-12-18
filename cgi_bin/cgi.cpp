#include "cgi.hpp"

int i = 0;

void debud_cgi_data(cgi_data &cgi)
{
    std::cout << GREEN << "cgi.body: " << cgi.body << std::endl;
    std::cout << "cgi.status_code: " << cgi.status_code << std::endl;
    std::cout << "cgi.status_message: " << cgi.status_message << std::endl;
    std::cout << "cgi.cgi_response: " << cgi.cgi_response << std::endl;
    std::cout << "cgi.cgi_headers: " << std::endl;
    for (std::map<std::string, std::string>::iterator it = cgi.cgi_headers.begin(); it != cgi.cgi_headers.end(); it++)
    {
        std::cout << it->first << ": " << it->second << RESET << std::endl;
    }
    std::cout << YELLOW << cgi.env_server.SERVER_SOFTWARE << std::endl;
    std::cout << cgi.env_server.REQUEST_METHOD << std::endl;
    std::cout << cgi.env_server.QUERY_STRING << std::endl;
    std::cout << cgi.env_server.CONTENT_TYPE << std::endl;
    std::cout << cgi.env_server.CONTENT_LENGTH << std::endl;
    std::cout << cgi.env_server.SCRIPT_FILENAME << std::endl;
    std::cout << cgi.env_server.SCRIPT_NAME << std::endl;
    std::cout << cgi.env_server.REQUEST_URI << std::endl;
    std::cout << cgi.env_server.DOCUMENT_ROOT << std::endl;
    std::cout << cgi.env_server.DOCUMENT_URI << std::endl;
    std::cout << cgi.env_server.SERVER_PROTOCOL << std::endl;
    std::cout << cgi.env_server.SERVER_ADDR << std::endl;
    std::cout << cgi.env_server.SERVER_PORT << std::endl;
    std::cout << cgi.env_server.SERVER_NAME << std::endl;
    std::cout << cgi.env_server.PATH_INFO << std::endl;
    std::cout << cgi.env_server.PATH_TRANSLATED << std::endl;
    std::cout << cgi.env_server.REDIRECT_STATUS << std::endl;
    std::cout << cgi.env_server.REDIRECT_STATUS_ << std::endl;
    std::cout << cgi.env_server.HTTP_COOKIE << RESET << std::endl;
}

cgi_data GET_CGI_DATA(http_items &response_items)
{
    envirmoment env_server = GET_SETENV_SERVER(response_items);

    cgi_data cgi(response_items, env_server, response_items.Body);
    char **envs = GET_EXTRA_ENV(cgi.env_server);
    std::string R = EXEC_CGI(cgi, envs);
    // debud_cgi_data(cgi);
    return cgi;
}

envirmoment GET_SETENV_SERVER(http_items &response_items)
{
    envirmoment env;
    if ((response_items.Path.substr(response_items.Path.find_last_of(".") + 1)) == "py")
        env.SCRIPT_FILENAME = "SCRIPT_FILENAME=" + response_items.location->cgi_path;
    else if ((response_items.Path.substr(response_items.Path.find_last_of(".") + 1)) == "php")
        env.SCRIPT_FILENAME = "SCRIPT_FILENAME=" + response_items.location->cgi_path;
    env.SERVER_SOFTWARE = "SERVER_SOFTWARE=WEBSERV42";
    env.REQUEST_METHOD = "REQUEST_METHOD=" + response_items.method;
    env.QUERY_STRING = "QUERY_STRING=" + response_items.Query_String;
    env.CONTENT_TYPE = "CONTENT_TYPE=" + response_items.Headers.find("Content-Type")->second;
    env.CONTENT_LENGTH = "CONTENT_LENGTH=" + (response_items.Headers.find("Content-Length")->second == "" ? "1024" : response_items.Headers.find("Content-Length")->second);
    env.SCRIPT_NAME = "SCRIPT_NAME=" + response_items.location->root + response_items.Path.substr(response_items.Path.find_first_of("/") + 1);
    env.REQUEST_URI = "REQUEST_URI=" + response_items.Path;
    env.DOCUMENT_ROOT = "DOCUMENT_ROOT=" + response_items.location->root;
    env.DOCUMENT_URI = "DOCUMENT_URI=" + response_items.Path;
    env.SERVER_PROTOCOL = "SERVER_PROTOCOL=HTTP/1.1";
    env.SERVER_ADDR = "SERVER_ADDR=" + response_items.server_name;
    env.SERVER_PORT = "SERVER_PORT=" + ntos(response_items.port);
    env.SERVER_NAME = "SERVER_NAME=" + response_items.server_name;
    env.REDIRECT_STATUS = "REDIRECT_STATUS=200";
    env.PATH_INFO = "PATH_INFO=/" + response_items.Path.substr(response_items.Path.find_last_of("/") + 1);
    env.PATH_TRANSLATED = "PATH_TRANSLATED=" + response_items.location->root + response_items.Path.substr(response_items.Path.find_last_of("/") + 1);
    if (response_items.Headers.find("Cookie") != response_items.Headers.end())
        env.HTTP_COOKIE = "HTTP_COOKIE=" + response_items.Headers.find("Cookie")->second;
    return env;
}

char **GET_EXTRA_ENV(envirmoment &env_server) // after this function, ENV_SERVER is ready to be use
{
    extern char **environ;
    char **extra_env;
    for (i = 0; environ[i] != NULL; i++)
        ;
    extra_env = new char *[i + env_server.NUM_ELEMENTS + 1];
    for (int j = 0; j < i; j++)
    {
        extra_env[j] = new char[strlen(environ[j]) + 1];
        strcpy(extra_env[j], environ[j]);
    }
    extra_env[i] = new char[env_server.REQUEST_METHOD.size() + 1];
    strcpy(extra_env[i], env_server.REQUEST_METHOD.c_str());
    extra_env[i + 1] = new char[env_server.QUERY_STRING.size() + 1];
    strcpy(extra_env[i + 1], env_server.QUERY_STRING.c_str());
    extra_env[i + 2] = new char[env_server.CONTENT_TYPE.size() + 1];
    strcpy(extra_env[i + 2], env_server.CONTENT_TYPE.c_str());
    extra_env[i + 3] = new char[env_server.CONTENT_LENGTH.size() + 1];
    strcpy(extra_env[i + 3], env_server.CONTENT_LENGTH.c_str());
    extra_env[i + 4] = new char[env_server.SCRIPT_FILENAME.size() + 1];
    strcpy(extra_env[i + 4], env_server.SCRIPT_FILENAME.c_str());
    extra_env[i + 5] = new char[env_server.SCRIPT_NAME.size() + 1];
    strcpy(extra_env[i + 5], env_server.SCRIPT_NAME.c_str());
    extra_env[i + 6] = new char[env_server.REQUEST_URI.size() + 1];
    strcpy(extra_env[i + 6], env_server.REQUEST_URI.c_str());
    extra_env[i + 7] = new char[env_server.DOCUMENT_ROOT.size() + 1];
    strcpy(extra_env[i + 7], env_server.DOCUMENT_ROOT.c_str());
    extra_env[i + 8] = new char[env_server.DOCUMENT_URI.size() + 1];
    strcpy(extra_env[i + 8], env_server.DOCUMENT_URI.c_str());
    extra_env[i + 9] = new char[env_server.SERVER_PROTOCOL.size() + 1];
    strcpy(extra_env[i + 9], env_server.SERVER_PROTOCOL.c_str());
    extra_env[i + 10] = new char[env_server.SERVER_ADDR.size() + 1];
    strcpy(extra_env[i + 10], env_server.SERVER_ADDR.c_str());
    extra_env[i + 11] = new char[env_server.SERVER_PORT.size() + 1];
    strcpy(extra_env[i + 11], env_server.SERVER_PORT.c_str());
    extra_env[i + 12] = new char[env_server.SERVER_NAME.size() + 1];
    strcpy(extra_env[i + 12], env_server.SERVER_NAME.c_str());
    extra_env[i + 13] = new char[env_server.SERVER_SOFTWARE.size() + 1];
    strcpy(extra_env[i + 13], env_server.SERVER_SOFTWARE.c_str());
    extra_env[i + 14] = new char[env_server.REDIRECT_STATUS.size() + 1];
    strcpy(extra_env[i + 14], env_server.REDIRECT_STATUS.c_str());
    extra_env[i + 15] = new char[env_server.PATH_INFO.size() + 1];
    strcpy(extra_env[i + 15], env_server.PATH_INFO.c_str());
    extra_env[i + 16] = new char[env_server.PATH_TRANSLATED.size() + 1];
    strcpy(extra_env[i + 16], env_server.PATH_TRANSLATED.c_str());
    extra_env[i + 17] = new char[env_server.HTTP_COOKIE.size() + 1];
    strcpy(extra_env[i + 17], env_server.HTTP_COOKIE.c_str());
    extra_env[i + 18] = NULL;
    return extra_env;
}
