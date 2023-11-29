#include "cgi.hpp"

int i = 0;

void debud_cgi_data(cgi_data &cgi)
{
    std::cout << YELLOW << "SERVER_SOFTWARE: " << cgi.env_server.SERVER_SOFTWARE << std::endl;
    std::cout << "SCRIPT_FILENAME: " << cgi.env_server.SCRIPT_FILENAME << std::endl;
    std::cout << "SERVER_PROTOCOL: " << cgi.env_server.SERVER_PROTOCOL << std::endl;
    std::cout << "REQUEST_METHOD: " << cgi.env_server.REQUEST_METHOD << std::endl;
    std::cout << "CONTENT_LENGTH: " << cgi.env_server.CONTENT_LENGTH << std::endl;
    std::cout << "DOCUMENT_ROOT: " << cgi.env_server.DOCUMENT_ROOT << std::endl;
    std::cout << "QUERY_STRING: " << cgi.env_server.QUERY_STRING << std::endl;
    std::cout << "CONTENT_TYPE: " << cgi.env_server.CONTENT_TYPE << std::endl;
    std::cout << "DOCUMENT_URI: " << cgi.env_server.DOCUMENT_URI << std::endl;
    std::cout << "SCRIPT_NAME: " << cgi.env_server.SCRIPT_NAME << std::endl;
    std::cout << "REQUEST_URI: " << cgi.env_server.REQUEST_URI << std::endl;
    std::cout << "SERVER_ADDR: " << cgi.env_server.SERVER_ADDR << std::endl;
    std::cout << "SERVER_PORT: " << cgi.env_server.SERVER_PORT << std::endl;
    std::cout << "SERVER_NAME: " << cgi.env_server.SERVER_NAME << std::endl;
    std::cout << "body: " << cgi.body << RESET << std::endl;
}

cgi_data GET_CGI_DATA(http_items &response_items)
{
    cgi_data cgi;
    cgi.env_server = GET_SETENV_SERVER(response_items);
    cgi.body = response_items.Body;
    return cgi;
}

envirmoment GET_SETENV_SERVER(http_items &response_items)
{
    envirmoment env;
    env.SERVER_SOFTWARE = "SERVER_SOFTWARE=WEBSERV42";
    env.REQUEST_METHOD = "REQUEST_METHOD=" + response_items.method;
    env.QUERY_STRING = "QUERY_STRING=" + response_items.Query_String;
    env.CONTENT_TYPE = "CONTENT_TYPE=" + response_items.Headers.find("Host")->second;
    env.CONTENT_LENGTH = "CONTENT_LENGTH=" + (response_items.Headers.find("Content-Length")->second == "" ? "1024" : response_items.Headers.find("Content-Length")->second);
    env.SCRIPT_FILENAME = "SCRIPT_FILENAME=" + response_items.location->root + response_items.Path;
    env.SCRIPT_NAME = "SCRIPT_NAME=" + response_items.Path.substr(response_items.Path.find_last_of("/") + 1);
    env.REQUEST_URI = "REQUEST_URI=" + response_items.Path;
    env.DOCUMENT_ROOT = "DOCUMENT_ROOT=" + response_items.location->root;
    env.DOCUMENT_URI = "DOCUMENT_URI=" + response_items.Path;
    env.SERVER_PROTOCOL = "SERVER_PROTOCOL=HTTP/1.1";
    env.SERVER_ADDR = "SERVER_ADDR=" + response_items.server_name;
    env.SERVER_PORT = std::to_string(response_items.port);
    env.SERVER_NAME = "SERVER_NAME=" + response_items.server_name;
    return env;
}

// void ADD_TOENV_SERVER(envirmoment &env_server) // after this function, ENV_SERVER is ready to be used
// {
//     extern char **environ;
//     for (i = 0; environ[i] != NULL; i++);
//     ENV_SERVER = new char*[i + env_server.NUM_ELEMENTS];
//     for (int j = 0; j < i; j++)
//     {
//         ENV_SERVER[j] = new char[strlen(environ[j]) + 1];
//         strcpy(ENV_SERVER[j], environ[j]);
//     }
//     ENV_SERVER[i] = new char[env_server.REQUEST_METHOD.size() + 1];
//     strcpy(ENV_SERVER[i], env_server.REQUEST_METHOD.c_str());
//     ENV_SERVER[i + 1] = new char[env_server.QUERY_STRING.size() + 1];
//     strcpy(ENV_SERVER[i + 1], env_server.QUERY_STRING.c_str());
//     ENV_SERVER[i + 2] = new char[env_server.CONTENT_TYPE.size() + 1];
//     strcpy(ENV_SERVER[i + 2], env_server.CONTENT_TYPE.c_str());
//     ENV_SERVER[i + 3] = new char[env_server.CONTENT_LENGTH.size() + 1];
//     strcpy(ENV_SERVER[i + 3], env_server.CONTENT_LENGTH.c_str());
//     ENV_SERVER[i + 4] = new char[env_server.SCRIPT_FILENAME.size() + 1];
//     strcpy(ENV_SERVER[i + 4], env_server.SCRIPT_FILENAME.c_str());
//     ENV_SERVER[i + 5] = new char[env_server.SCRIPT_NAME.size() + 1];
//     strcpy(ENV_SERVER[i + 5], env_server.SCRIPT_NAME.c_str());
//     ENV_SERVER[i + 6] = new char[env_server.REQUEST_URI.size() + 1];
//     strcpy(ENV_SERVER[i + 6], env_server.REQUEST_URI.c_str());
//     ENV_SERVER[i + 7] = new char[env_server.DOCUMENT_ROOT.size() + 1];
//     strcpy(ENV_SERVER[i + 7], env_server.DOCUMENT_ROOT.c_str());
//     ENV_SERVER[i + 8] = new char[env_server.DOCUMENT_URI.size() + 1];
//     strcpy(ENV_SERVER[i + 8], env_server.DOCUMENT_URI.c_str());
//     ENV_SERVER[i + 9] = new char[env_server.SERVER_PROTOCOL.size() + 1];
//     strcpy(ENV_SERVER[i + 9], env_server.SERVER_PROTOCOL.c_str());
//     ENV_SERVER[i + 10] = new char[env_server.SERVER_ADDR.size() + 1];
//     strcpy(ENV_SERVER[i + 10], env_server.SERVER_ADDR.c_str());
//     ENV_SERVER[i + 11] = new char[env_server.SERVER_PORT.size() + 1];
//     strcpy(ENV_SERVER[i + 11], env_server.SERVER_PORT.c_str());
//     ENV_SERVER[i + 12] = new char[env_server.SERVER_NAME.size() + 1];
//     strcpy(ENV_SERVER[i + 12], env_server.SERVER_NAME.c_str());
// }

// void FREEENV_SERVER(envirmoment &env_server)
// {
//     for (size_t j = 0; j < i + env_server.NUM_ELEMENTS; j++)
//     {
//         delete[] ENV_SERVER[j];
//     }
//     delete[] ENV_SERVER;
// }

// int main(int argc, char const *argv[],char **env)
// {
//     std:: string req = std::string(argv[1]);
//     envirmoment env_server = GET_SETENV_SERVER(req);
//     ADD_TOENV_SERVER(env_server, env);
//     for (int j = 0; j < i + env_server.NUM_ELEMENTS ; j++)
//     {
//         std::cout << ENV_SERVER[j] << std::endl;
//     }
//     FREEENV_SERVER(env_server);
//     system("leaks a.out");
//     return 0;
// }


// int main()
// {
//     cgi_data cgi;
//     return 0;
// }
