#include "cgi.hpp"

int i = 0;

void debud_cgi_data(cgi_data &cgi, char **extra_env)
{
    (void)cgi;
    for (size_t j = 0; extra_env[j]; j++)
    {
        std::cout << YELLOW << extra_env[j] << std::endl;
    }
    // std::cout << YELLOW << "SERVER_SOFTWARE: " << cgi.env_server.SERVER_SOFTWARE << std::endl;
    // std::cout << "SCRIPT_FILENAME: " << cgi.env_server.SCRIPT_FILENAME << std::endl;
    // std::cout << "SERVER_PROTOCOL: " << cgi.env_server.SERVER_PROTOCOL << std::endl;
    // std::cout << "REQUEST_METHOD: " << cgi.env_server.REQUEST_METHOD << std::endl;
    // std::cout << "CONTENT_LENGTH: " << cgi.env_server.CONTENT_LENGTH << std::endl;
    // std::cout << "DOCUMENT_ROOT: " << cgi.env_server.DOCUMENT_ROOT << std::endl;
    // std::cout << "QUERY_STRING: " << cgi.env_server.QUERY_STRING << std::endl;
    // std::cout << "CONTENT_TYPE: " << cgi.env_server.CONTENT_TYPE << std::endl;
    // std::cout << "DOCUMENT_URI: " << cgi.env_server.DOCUMENT_URI << std::endl;
    // std::cout << "SCRIPT_NAME: " << cgi.env_server.SCRIPT_NAME << std::endl;
    // std::cout << "REQUEST_URI: " << cgi.env_server.REQUEST_URI << std::endl;
    // std::cout << "SERVER_ADDR: " << cgi.env_server.SERVER_ADDR << std::endl;
    // std::cout << "SERVER_PORT: " << cgi.env_server.SERVER_PORT << std::endl;
    // std::cout << "SERVER_NAME: " << cgi.env_server.SERVER_NAME << std::endl;
    // std::cout << "body: " << cgi.body << RESET << std::endl;
}

cgi_data GET_CGI_DATA(http_items &response_items)
{
    cgi_data cgi(response_items, GET_SETENV_SERVER(response_items), response_items.Body);
    char **envs = GET_EXTRA_ENV(cgi.env_server);
    debud_cgi_data(cgi, envs);
    std::string R = EXEC_CGI(cgi, envs);
    std::cout << RED << R << RESET << std::endl;
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
    env.CONTENT_TYPE = "CONTENT_TYPE=" + response_items.Headers.find("Host")->second;
    env.CONTENT_LENGTH = "CONTENT_LENGTH=" + (response_items.Headers.find("Content-Length")->second == "" ? "1024" : response_items.Headers.find("Content-Length")->second);
    env.SCRIPT_NAME = "SCRIPT_NAME=" + response_items.location->root + response_items.Path.substr(response_items.Path.find_last_of("/") + 1);
    env.REQUEST_URI = "REQUEST_URI=" + response_items.Path;
    env.DOCUMENT_ROOT = "DOCUMENT_ROOT=" + response_items.location->root;
    env.DOCUMENT_URI = "DOCUMENT_URI=" + response_items.Path;
    env.SERVER_PROTOCOL = "SERVER_PROTOCOL=HTTP/1.1";
    env.SERVER_ADDR = "SERVER_ADDR=" + response_items.server_name;
    env.SERVER_PORT = std::to_string(response_items.port);
    env.SERVER_NAME = "SERVER_NAME=" + response_items.server_name;
    env.REDIRECT_STATUS = "REDIRECT_STATUS=200";
    env.PATH_INFO = "PATH_INFO=" + response_items.Path.substr(response_items.Path.find_last_of("/") + 1);
    env.PATH_TRANSLATED = "PATH_TRANSLATED=" + response_items.location->root + response_items.Path.substr(response_items.Path.find_last_of("/") + 1);
    return env;
}

char **GET_EXTRA_ENV(envirmoment &env_server) // after this function, ENV_SERVER is ready to be use
{
    extern char **environ;
    char **extra_env;
    for (i = 0; environ[i] != NULL; i++);
    extra_env = new char*[i + env_server.NUM_ELEMENTS + 1];
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
    extra_env[i + 17] = NULL;
    return extra_env;
}

std::string exec_get(cgi_data &cgi, char **extra_env)
{
    int cgi_pid;
    int cgi_output[2];
    int cgi_input[2];
    int status;
    char c;
    std::string cgi_response;
    if (pipe(cgi_output) < 0)
    {
        std::cout << "Error: pipe" << std::endl;
        return "";
    }
    if (pipe(cgi_input) < 0)
    {
        std::cout << "Error: pipe" << std::endl;
        return "";
    }
    if ((cgi_pid = fork()) < 0)
    {
        std::cout << "Error: fork" << std::endl;
        return "";
    }
    if (cgi_pid == 0)
    {
        dup2(cgi_output[1], 1);
        dup2(cgi_input[0], 0);
        close(cgi_output[0]);
        close(cgi_input[1]);
        const char *file = strdup(cgi.env_server.SCRIPT_FILENAME.substr(cgi.env_server.SCRIPT_FILENAME.find_last_of("=") + 1).c_str());
        const char *script_name = strdup(cgi.env_server.SCRIPT_NAME.substr(cgi.env_server.SCRIPT_NAME.find_last_of("=") + 1).c_str());
        char *const argv[] = {const_cast<char*>(file), const_cast<char*>(script_name), nullptr};  // Construct argv array
        execve(file, argv, extra_env);
        exit(0);
    }
    else
    {
        close(cgi_output[1]);
        close(cgi_input[0]);
        for (size_t j = 0; j < cgi.body.size(); j++)
        {
            c = cgi.body[j];
            write(cgi_input[1], &c, 1);
        }
        int sizeread = cgi.response_tools.Headers.find("Content-Length")->second == "" ? 1000000 : std::stoi(cgi.response_tools.Headers.find("Content-Length")->second);
        std::cout << "sizeread: " << sizeread << std::endl;
        while (read(cgi_output[0], &c, 1) > 0 || sizeread--) // TODO buffer read
        {
            cgi_response += c;
        }
        waitpid(cgi_pid, &status, 0);
        close(cgi_output[0]);
        close(cgi_input[1]);
    }
    return cgi_response;
}

std::string exec_post(cgi_data &cgi, char **extra_env)
{
    int cgi_pid;
    int post_in = open("post_in", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    int post_out = open("post_out", O_CREAT | O_RDONLY | O_TRUNC, 0644);
    int status;
    char c;
    std::string cgi_response;
    if (post_in < 0 || post_out < 0)
    {
        std::cout << "Error: open" << std::endl;
        return "";
    }
    for (size_t j = 0; j < cgi.body.size(); j++)
    {
        c = cgi.body[j];
        write(post_in, &c, 1);
    }
    if ((cgi_pid = fork()) < 0)
    {
        std::cout << "Error: fork" << std::endl;
        return "";
    }
    if (cgi_pid == 0)
    {
        dup2(post_out, 0);
        dup2(post_in, 1);
        close(post_out);
        close(post_in);
        const char *file = strdup(cgi.env_server.SCRIPT_FILENAME.substr(cgi.env_server.SCRIPT_FILENAME.find_last_of("=") + 1).c_str());
        const char *script_name = strdup(cgi.env_server.SCRIPT_NAME.substr(cgi.env_server.SCRIPT_NAME.find_last_of("=") + 1).c_str());
        char *const argv[] = {const_cast<char*>(file), const_cast<char*>(script_name), nullptr};  // Construct argv array
        execve(file, argv, extra_env);
        WIFEXITED(status);
    }
    else
    {
        close(post_out);
        close(post_in);
        for (size_t j = 0; j < cgi.body.size(); j++)
        {
            c = cgi.body[j];
            write(post_in, &c, 1);
        }
        waitpid(cgi_pid, &status, 0);
        // int sizeread = cgi.response_tools.Headers.find("Content-Length")->second == "" ? 1024 : std::stoi(cgi.response_tools.Headers.find("Content-Length")->second);
        while (read(post_out, &c, 1) > 0 )
        {
            cgi_response += c;
        }
        close(post_out);
        close(post_in);
    }
    return cgi_response;
}

std::string EXEC_CGI(cgi_data &cgi, char **extra_env)
{
    if (cgi.env_server.REQUEST_METHOD == "REQUEST_METHOD=GET")
        return exec_get(cgi, extra_env);
    else if (cgi.env_server.REQUEST_METHOD == "REQUEST_METHOD=POST")
        return exec_post(cgi, extra_env);
    return ""; //TODO: sql or other
}

std::string CONVERT_TO_WEB(std::string &response)
{
    return response;
    //TODO: convert cgi response to web response
}


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
//     GET_EXTRA_ENV(env_server, env);
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