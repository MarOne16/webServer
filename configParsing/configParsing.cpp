#include "configParsing.hpp"

unsigned int alarmCounter = INT_MAX;

ConfigParser::ConfigParser(const char **argv)
{
    setConfKeys();
    if (argv[1])
    {
        isArgv = true;
        std::ifstream file(argv[1]);
        if (!file.is_open())
            throw std::runtime_error("File not found.");
        file.close();
        m_fileName = (char *)argv[1];
    }
    else
    {
        isArgv = false;
        m_fileName = getFileName(".conf");
        if (!m_fileName)
            throw std::runtime_error("No .conf file found in the directory.");
    }
}

ConfigParser::~ConfigParser()
{
    // closedir(dir);
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
        this->servers_content += line;
        this->servers_content += '\n';
    }
    file.close();
}

void ConfigParser::feedContent()
{
    if (this->servers_content.find("server") == std::string::npos)
        throw std::runtime_error("No server directive found.");
    size_t pos = this->servers_content.find("server");
    std::string server = "";
    for (size_t i = pos; i < this->servers_content.length(); i++)
    {
        if (servers_content[i] == '}')
        {
            server += servers_content[i];
            break;
        }
        server += servers_content[i];
    }
    ereaseContent(this->servers_content, pos, '}');
    this->content.clear();
    this->content = server;
    feedServers();

}

void ConfigParser::checkBrackets()
{
    int left = 0;
    int right = 0;
    for (size_t i = 0; i < this->servers_content.length(); i++)
    {
        if (this->servers_content[i] == '{')
            left++;
        if (this->servers_content[i] == '}')
            right++;
    }
    if (left != right)
        throw std::runtime_error("Brackets are not balanced.");
    // send line by line to check_if_in_confKeys
    std::string line;
    for (size_t i = 0; i < this->servers_content.length(); i++)
    {
        if (this->servers_content[i] != '\n' && this->servers_content[i] != '\0')
        {
            line += this->servers_content[i];
            continue;
        }
        check_if_in_confKeys(getKey(line));
        line.clear();
    }
    feedContent();
}

void ConfigParser::feedServers()
{
    static int i;
    globalUpload();
    setAlarm();
    server server_tmp;
    server_tmp.port = getPort();
    server_tmp.server_name = getServerName();
    server_tmp.host = getHost();
    server_tmp.max_body_size = getMaxBodySize();
    server_tmp.error_pages = getErrorPages();
    server_tmp.default_location = getRootServ();
    feedLocations();
    server_tmp.locations = m_locations;
    m_servers[i++] = server_tmp;
    if (this->servers_content.find("server") != std::string::npos)
        feedContent();
    // closedir(dir);
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

int      ConfigParser::getPort()
{
    std::string port = "";
    if (!ifInside("server", "listen"))
        throw std::runtime_error("No listen directive found.");
    size_t pos = this->content.find("listen");
    for (size_t i = pos + 7; i < this->content.length(); i++)
    {
        if (content[i] == ';')
        {
            if (port.empty())
                throw std::runtime_error("Port is empty.");
            port += content[i];
            break;
        }
        port += content[i];
    }
    if (!ifClosed(port))
        throw std::runtime_error("Listen directive is not closed.");
    ereaseContent(this->content, pos, ';');
    if (ifInside("server", "listen"))
        throw std::runtime_error("multiple listen directive not allowed.");
    return toInt(port.erase(port.length() - 1, 1));
}

std::string ConfigParser::getServerName()
{
    if (!ifInside("server", "server_name"))
        throw std::runtime_error("No server_name directive found.");
    size_t pos = this->content.find("server_name");
    std::string serverName = "";
    for (size_t i = pos + 11; i < this->content.length(); i++)
    {
        if (content[i] == ';')
        {
            if (serverName.empty())
                throw std::runtime_error("Server name is empty.");
            serverName += content[i];
            break;
        }
        serverName += content[i];
    }
    if (!ifClosed(serverName))
        throw std::runtime_error("Server name directive is not closed.");
    serverName.erase(serverName.length() - 1, 1);
    return serverName;
}

std::string ConfigParser::getHost()
{
    if (!ifInside("server", "host"))
        return "0.0.0.0";
    size_t pos = this->content.find("host");
    std::string host = "";
    for (size_t i = pos + 4; i < this->content.length(); i++)
    {
        if (content[i] == ' ' || content[i] == '\t')
            continue;
        if (content[i] == ';')
        {
            host += content[i];
            break;
        }
        host += content[i];
    }
    if (!ifClosed(host))
        throw std::runtime_error("Host directive is not closed.");
    host = convertDomainToIPv4(host.erase(host.length() - 1, 1));
    if (host.empty())
        throw std::runtime_error("Host is not valid.");
    return host;
}

std::string ConfigParser::getMaxBodySize()
{
    if (!ifInside("server", "max_body_size"))
        return "1000000";
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
    if (notIn(maxBodySize, "0123456789 "))
        throw std::runtime_error("Max body size is not valid excepted only numbers.\nKeeping mind 1 = 1 byte.");
    if (maxBodySize.length() > 10)
        throw std::runtime_error("Max body size too big.");
    return maxBodySize;
}

std::map<std::string, std::string> ConfigParser::getErrorPages()
{
    std::map<std::string, std::string> errorPages;
    start:
    if (!ifInside("server", "error_page"))
        return errorPages;
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
        throw std::runtime_error("Error page is not valid excepted only .html files.");
    for (; it != ite; it++)
    {
        if (it == last)
            break;
        if (notIn(*it, "0123456789") || (*it).length() != 3)
            throw std::runtime_error("Error page: satus request is not valid.");
        errorPages[*it] = *last;
    }
    if (this->content.find("error_page") != std::string::npos)
        goto start;
    return errorPages;
}

unsigned int ConfigParser::getNumber_ofServers()
{
    return m_servers.size();
}

std::string ConfigParser::getRootServ()
{
    if (content.find("root") == std::string::npos || !ifOutsideLocation("root"))
    {
        global_root = getDefault("root");
        return (global_root);
    }
    if (ifOutsideLocation("root"))
    {
        std::string root = "";
        size_t pos = content.find("root");
        for (size_t i = pos + 4; i < content.length(); i++)
        {
            if (content[i] == ' ' || content[i] == '\t')
                continue;
            if (content[i] == ';' || content[i] == '\n')
            {
                if (content[i] == ';')
                    root += content[i];
                break;
            }
            root += content[i];
        }
        if (!ifClosed(root))
            throw std::runtime_error("Root directive is not closed.");
        ereaseContent(content, pos, ';');
        if (!findFile(root.erase(root.length() - 1, 1)))
            throw std::runtime_error("Root is not valid.");
        global_root = root;
        if (global_root.find_last_of('/') != global_root.length() - 1)
            global_root += '/';
    }
    if (global_root.empty())
        throw std::runtime_error("Root is empty.");
    return global_root;
}

bool ConfigParser::ifOutsideLocation(std::string line)
{
    size_t first_pos = content.find(line);
    size_t second_pos = first_pos + line.length();
    for (size_t i = 0; i < content.length(); i++)
    {
        size_t first = 0;
        size_t second = 0;
        if (content[i] == '(')
        {
            first = i;
            for (size_t j = i; j < content.length(); j++)
            {
                if (content[j] == ')')
                {
                    second = j;
                    break;
                }
            }
            if (first < first_pos && second > second_pos)
                return false;
            if (second != 0)
                i = second;
        }
    }
    return true;
}

void ConfigParser::globalUpload()
{
    if (content.find("upload_store_directory") == std::string::npos)
    {
        global_upload_store = getDefault("upload_store_directory");
        return;
    }
    if (ifOutsideLocation("upload_store_directory"))
    {
        std::string upload = "";
        size_t pos = content.find("upload_store_directory");
        for (size_t i = pos + 22; i < content.length(); i++)
        {
            if (content[i] == ' ' || content[i] == '\t')
                continue;
            if (content[i] == ';' || content[i] == '\n')
            {
                if (content[i] == ';')
                    upload += content[i];
                break;
            }
            upload += content[i];
        }
        if (!ifClosed(upload))
            throw std::runtime_error("upload directive is not closed.");
        if (!findFile(upload.erase(upload.length() - 1, 1)))
            throw std::runtime_error("upload_store_directory is not valid.");
        ereaseContent(content, pos, ';');
        if (upload.find_last_of('/') != upload.length() - 1)
            upload += '/';
        global_upload_store = upload;
    }
}

void ConfigParser::setAlarm()
{
    if (content.find("alarm") == std::string::npos)
        return;
    size_t pos = content.find("alarm");
    std::string alarm = "";
    for (size_t i = pos + 5; i < content.length(); i++)
    {
        if (content[i] == ';')
            break;
        alarm += content[i];
    }
    for (size_t i = 0; i < alarm.length(); i++)
    {
        if (alarm[i] == ' ' || alarm[i] == '\t' || alarm[i] == '\n')
            alarm.erase(i--, 1);
    }
    if (notIn(alarm, "0123456789"))
        throw std::runtime_error("Alarm is not valid excepted only numbers.");
    if (alarm.length() > 10)
        throw std::runtime_error("Alarm too big.");
    alarmCounter = toInt(alarm);
    if (alarmCounter < 0)
        throw std::runtime_error("Alarm is negative.");
}

void ConfigParser::closeDir()
{
    if (isArgv == false)
        closedir(dir);
}