#include "cgi.hpp"

void FreeENV(char **env)
{
    int i = 0;
    while (env[i] != NULL)
    {
        delete[] env[i];
        i++;
    }
    delete[] env;
}

void exec(cgi_data &cgi, char **extra_env, std::string method)
{
    int out = open("/tmp/cgi_out", O_RDWR | O_CREAT | O_TRUNC, 0777);
    int in = open("/tmp/cgi_in", O_RDWR | O_CREAT | O_TRUNC, 0777);
    int status;
    int pid = fork();
    if (out == -1 || in == -1 || pid == -1)
    {
        cgi.status_code = "500";
        cgi.cgi_response = "Error: external program execution failed";
        cgi.status_message = "Internal Server Error";
        return;
    }
    if (pid == 0)
    {
        if (method == "REQUEST_METHOD=POST")
        {
            write(in, cgi.body.c_str(), cgi.body.length());
            lseek(in, 0, SEEK_SET);
        }
        dup2(out, 1);
        dup2(in, 0);
        close(in);
        close(out);
        alarm(alarmCounter);
        const char *file = strdup(cgi.env_server.SCRIPT_FILENAME.substr(cgi.env_server.SCRIPT_FILENAME.find_last_of("=") + 1).c_str());
        const char *script_name = strdup(cgi.env_server.SCRIPT_NAME.substr(cgi.env_server.SCRIPT_NAME.find_last_of("=") + 1).c_str());
        char *const argv[] = {const_cast<char *>(file), const_cast<char *>(script_name), nullptr}; // Construct argv array
        execve(file, argv, extra_env);
        exit(1);
    }
    else
    {
        close(out);
        close(in);
        waitpid(pid, &status, 0);
        FreeENV(extra_env);
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) != 0)
            {
                if (WIFSIGNALED(status))
                {
                    if (WTERMSIG(status) == SIGALRM)
                    {
                        cgi.status_code = "504";
                        cgi.cgi_response = "Error: Getaway Timeout";
                        cgi.status_message = "Getaway Timeout";
                        remove("/tmp/cgi_out");
                        return;
                    }
                }
                cgi.status_code = "500";
                cgi.cgi_response = "Error: external program execution failed";
                cgi.status_message = "Internal Server Error";
                return;
            }
            else
            {
                cgi.status_code = "200";
            }
        }
        else
        {
            if (WIFSIGNALED(status))
            {
                if (WTERMSIG(status) == SIGALRM)
                {
                    cgi.status_code = "413";
                    cgi.cgi_response = "Error: Request Entity Too Large";
                    cgi.status_message = "Request Entity Too Large";
                    remove("/tmp/cgi_out");
                    return;
                }
            }
            cgi.status_code = "500";
            cgi.cgi_response = "Error: external program execution failed";
            cgi.status_message = "Internal Server Error";
            return;
        }
        std::ifstream ifs("/tmp/cgi_out");
        if (ifs)
        {
            std::stringstream buffer;
            buffer << ifs.rdbuf();
            cgi.cgi_response = buffer.str();
        }
        else
        {
            cgi.status_code = "500";
            cgi.cgi_response = "Error: external program execution failed";
            cgi.status_message = "Internal Server Error";
            return;
        }
        ifs.close();
    }
    remove("/tmp/cgi_out");
    remove("/tmp/cgi_in");
}

std::string EXEC_CGI(cgi_data &cgi, char **extra_env)
{
    std::string response = "";
    exec(cgi, extra_env, cgi.env_server.REQUEST_METHOD);
    return cgi.cgi_response;
}
