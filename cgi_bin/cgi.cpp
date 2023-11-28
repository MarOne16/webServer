#include "cgi.hpp"

int i = 0;

envirmoment GET_ENV_SERVER(std::string &request)
{
    envirmoment env;
    env.REQUEST_METHOD = "REQUEST_METHOD";
    env.QUERY_STRING = "QUERY_STRING";
    env.CONTENT_TYPE = "CONTENT_TYPE";
    env.CONTENT_LENGTH = "CONTENT_LENGTH";
    env.SCRIPT_FILENAME = "SCRIPT_FILENAME";
    env.SCRIPT_NAME = "SCRIPT_NAME";
    env.REQUEST_URI = "REQUEST_URI";
    env.DOCUMENT_ROOT = "DOCUMENT_ROOT";
    env.DOCUMENT_URI = "DOCUMENT_URI";
    env.SERVER_PROTOCOL = "SERVER_PROTOCOL";
    env.SERVER_ADDR = "SERVER_ADDR";
    env.SERVER_PORT = "SERVER_PORT";
    env.SERVER_NAME = "SERVER_NAME";
    std::istringstream iss(request);
        std::vector<std::string> tokens;
        copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            back_inserter(tokens));

    // Check if the number of tokens matches the number of struct elements
    if (tokens.size() != env.NUM_ELEMENTS) {
        // Handle error: The input string doesn't match the expected format
        std::cerr << "Error: Invalid input format." << std::endl;
        exit(1);
    }
    env.REQUEST_METHOD += "=" + tokens[0];
    env.QUERY_STRING += "=" + tokens[1];
    env.CONTENT_TYPE += "=" + tokens[2];
    env.CONTENT_LENGTH += "=" + tokens[3];
    env.SCRIPT_FILENAME += "=" + tokens[4];
    env.SCRIPT_NAME += "=" + tokens[5];
    env.REQUEST_URI += "=" + tokens[6];
    env.DOCUMENT_ROOT += "=" + tokens[7];
    env.DOCUMENT_URI += "=" + tokens[8];
    env.SERVER_PROTOCOL += "=" + tokens[9];
    env.SERVER_ADDR += "=" + tokens[10];
    env.SERVER_PORT += "=" + tokens[11];
    env.SERVER_NAME += "=" + tokens[12];
    return env;
}

void ADD_TO_ENV_SERVER(envirmoment &env_server, char **env) // after this function, _ENV_SERVER is ready to be used
{
    for (i = 0; env[i] != NULL; i++);
    _ENV_SERVER = new char*[i + env_server.NUM_ELEMENTS];
    for (int j = 0; j < i; j++)
    {
        _ENV_SERVER[j] = new char[strlen(env[j]) + 1];
        strcpy(_ENV_SERVER[j], env[j]);
    }
    _ENV_SERVER[i] = new char[env_server.REQUEST_METHOD.size() + 1];
    strcpy(_ENV_SERVER[i], env_server.REQUEST_METHOD.c_str());
    _ENV_SERVER[i + 1] = new char[env_server.QUERY_STRING.size() + 1];
    strcpy(_ENV_SERVER[i + 1], env_server.QUERY_STRING.c_str());
    _ENV_SERVER[i + 2] = new char[env_server.CONTENT_TYPE.size() + 1];
    strcpy(_ENV_SERVER[i + 2], env_server.CONTENT_TYPE.c_str());
    _ENV_SERVER[i + 3] = new char[env_server.CONTENT_LENGTH.size() + 1];
    strcpy(_ENV_SERVER[i + 3], env_server.CONTENT_LENGTH.c_str());
    _ENV_SERVER[i + 4] = new char[env_server.SCRIPT_FILENAME.size() + 1];
    strcpy(_ENV_SERVER[i + 4], env_server.SCRIPT_FILENAME.c_str());
    _ENV_SERVER[i + 5] = new char[env_server.SCRIPT_NAME.size() + 1];
    strcpy(_ENV_SERVER[i + 5], env_server.SCRIPT_NAME.c_str());
    _ENV_SERVER[i + 6] = new char[env_server.REQUEST_URI.size() + 1];
    strcpy(_ENV_SERVER[i + 6], env_server.REQUEST_URI.c_str());
    _ENV_SERVER[i + 7] = new char[env_server.DOCUMENT_ROOT.size() + 1];
    strcpy(_ENV_SERVER[i + 7], env_server.DOCUMENT_ROOT.c_str());
    _ENV_SERVER[i + 8] = new char[env_server.DOCUMENT_URI.size() + 1];
    strcpy(_ENV_SERVER[i + 8], env_server.DOCUMENT_URI.c_str());
    _ENV_SERVER[i + 9] = new char[env_server.SERVER_PROTOCOL.size() + 1];
    strcpy(_ENV_SERVER[i + 9], env_server.SERVER_PROTOCOL.c_str());
    _ENV_SERVER[i + 10] = new char[env_server.SERVER_ADDR.size() + 1];
    strcpy(_ENV_SERVER[i + 10], env_server.SERVER_ADDR.c_str());
    _ENV_SERVER[i + 11] = new char[env_server.SERVER_PORT.size() + 1];
    strcpy(_ENV_SERVER[i + 11], env_server.SERVER_PORT.c_str());
    _ENV_SERVER[i + 12] = new char[env_server.SERVER_NAME.size() + 1];
    strcpy(_ENV_SERVER[i + 12], env_server.SERVER_NAME.c_str());
}

void FREE_ENV_SERVER(envirmoment &env_server)
{
    for (int j = 0; j < i + env_server.NUM_ELEMENTS; j++)
    {
        delete[] _ENV_SERVER[j];
    }
    delete[] _ENV_SERVER;
}

int main(int argc, char const *argv[],char **env)
{
    std:: string req = std::string(argv[1]);
    envirmoment env_server = GET_ENV_SERVER(req);
    ADD_TO_ENV_SERVER(env_server, env);
    for (int j = 0; j < i + env_server.NUM_ELEMENTS ; j++)
    {
        std::cout << _ENV_SERVER[j] << std::endl;
    }
    FREE_ENV_SERVER(env_server);
    system("leaks a.out");
    return 0;
}
