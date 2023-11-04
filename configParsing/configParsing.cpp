#include "configParsing.hpp"

ConfigParser::ConfigParser(const char **argv)
{
    if (argv[1])
    {
        std::ifstream file(argv[1]);
        if (!file.is_open())
            throw std::runtime_error("File not found.");
        file.close();
        m_fileName = (char *)argv[1];
    }
    else
    {
        m_fileName = getFileName(".conf");
        if (!m_fileName)
            throw std::runtime_error("No .conf file found in the directory.");
    }
}

ConfigParser::~ConfigParser()
{
    closedir(dir);
    // puts("ConfigParser destructor called.");
    // system("leaks a.out");
}

void ConfigParser::readConfigFile()
{
    std::ifstream file(m_fileName);
    std::string line;

    while (std::getline(file, line))
    {
        for (size_t i = 0; i < line.length(); i++)
        {
            if ((line[i] == ' ' || line[i] == '\t'))
                line.erase(i--, 1);
            if (line[i + 1] != ' ' && line[i + 1])
                break;
        }
        if (line[0] == '#' || line.empty())
            continue;
        this->content += line;
        this->content += '\n';
    }
}

void ConfigParser::checkBrackets()
{
    int left = 0;
    int right = 0;
    for (size_t i = 0; i < this->content.length(); i++)
    {
        if (this->content[i] == '{')
            left++;
        if (this->content[i] == '}')
            right++;
    }
    if (left != right)
        throw std::runtime_error("Brackets are not balanced.");
}

void ConfigParser::feedConfMap()
{
    std::string line;
}

bool ConfigParser::ifInside(std::string scope, std::string toFind)
{
    if (!isValideScope(scope))
        throw std::runtime_error("Scope is not valid.");
    std::string scopeTmp = content.substr(content.find(scope));
    size_t endScop = scopeTmp.find('}');
    std::string tmp = scopeTmp.substr(0, endScop);
    if (tmp.find(toFind) != std::string::npos)
        return true;
    return false;
}


bool ConfigParser::isValideScope(std::string scope)
{
    std::string scopeTmp = content.substr(content.find(scope));
    size_t endScop = scopeTmp.find('}');
    size_t startScop = (scope.length());
    bool find = false;
    for (size_t i = startScop; i <= endScop; i++)
    {
        if (scopeTmp[i] == ' ' || scopeTmp[i] == '\t' || scopeTmp[i] == '\n')
            continue;
        if (scopeTmp[i] == '{' && find == false)
        {
            find = true;
            continue;
        }
        if (scopeTmp[i] == '}' && find == true)
            return true;
    }
    return false;
}

int ConfigParser::getPort()
{
    std::string port = "";
    if (!ifInside("server", "listen"))
        throw std::runtime_error("No listen directive found.");
    size_t pos = this->content.find("listen");
    for (size_t i = pos + 7 ; i < this->content.length(); i++)
    {
        if (content[i] == ';')
            break;
        port += content[i];
    }
    return toInt(port);

}

std::string ConfigParser::getServerName()
{
    if (!ifInside("server", "server_name"))
        return "localhost";
    size_t pos = this->content.find("server_name");
    std::string serverName = "";
    for (size_t i = pos + 12; i < this->content.length(); i++)
    {
        if (content[i] == ';')
            break;
        serverName += content[i];
    }
    for (size_t i = 0; i < serverName.length(); i++)
    {
        if (serverName[i] == ' ' || serverName[i] == '\t' || serverName[i] == '\n')
            serverName.erase(i--, 1);
    }
    for (size_t i = 0; i < serverName.length(); i++)
    {
        if(isalnum(serverName[i]) || serverName[i] == '.' || serverName[i] == '-' || serverName[i] == '_')
            continue;
        else
            throw std::runtime_error("Server name is not valid.");
    }
    return serverName;
}

std::string ConfigParser::getHost()
{
    if (!ifInside("server", "host"))
        return "localhost";
    size_t pos = this->content.find("host");
    std::string host = "";
    for (size_t i = pos + 4; i < this->content.length(); i++)
    {
        if (content[i] == ';')
            break;
        host += content[i];
    }
    for (size_t i = 0; i < host.length(); i++)
    {
        if (host[i] == ' ' || host[i] == '\t' || host[i] == '\n')
            host.erase(i--, 1);
    }
    for (size_t i = 0; i < host.length(); i++)
    {
        if(isalnum(host[i]) || host[i] == '.' || host[i] == '-' || host[i] == '_')
            continue;
        else
            throw std::runtime_error("Host is not valid.");
    }
    return host;
}

std::string ConfigParser::getMaxBodySize()
{
    if (!ifInside("server", "max_body_size"))
        throw std::runtime_error("No max body size directive found.");
    size_t pos = this->content.find("max_body_size");
    std::string maxBodySize = "";
    for (size_t i = pos + 13; i < this->content.length(); i++)
    {
        if (content[i] == ';')
            break;
        maxBodySize += content[i];
    }
    for (size_t i = 0; i < maxBodySize.length(); i++)
    {
        if (maxBodySize[i] == ' ' || maxBodySize[i] == '\t' || maxBodySize[i] == '\n')
            maxBodySize.erase(i--, 1);
    }
    if (notIn(maxBodySize, "0123456789 mM"))
        throw std::runtime_error("Max body size is not valid.");
    return maxBodySize;
}

std::map<std::string , std::string> ConfigParser::getErrorPages()
{
    std::map<std::string, std::string> errorPages;
    start:
    if (!ifInside("server", "error_page"))
        throw std::runtime_error("No error page directive found.");
    size_t pos = this->content.find("error_page");
    std::string errorPage = "";
    for (size_t i = pos + 10; i < this->content.length(); i++)
    {
        if (content[i] == ';')
            break;
        errorPage += content[i];
    }
    ereaseContent(this->content, pos, ';');
    std::list<std::string> errorPageList = split(errorPage, " ");
    std::list<std::string>::iterator it = errorPageList.begin();
    std::list<std::string>::iterator ite = errorPageList.end();
    std::list<std::string>::iterator last = --errorPageList.end();
    if ((*last).find(".html") == std::string::npos)
        throw std::runtime_error("Error page is not valid.");
    for (; it != ite; it++)
    {
        if (it == last)
            break;
        if (notIn(*it, "0123456789") || (*it).length() != 3)
            throw std::runtime_error("Error page is not valid.");
        errorPages[*it] = *last;
    }
    if (this->content.find("error_page") != std::string::npos)
        goto start;
    return errorPages;
}