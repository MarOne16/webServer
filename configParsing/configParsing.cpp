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
    for (int i = 0; i < this->content.length(); i++)
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

int ConfigParser::getPort()
{
    std::string port = "";
    std::string server = content.substr(content.find("server"));
    if (!ifInside("server", "listen"))
        throw std::runtime_error("No listen directive found.");
    size_t pos = this->content.find("listen");
    if (pos == std::string::npos)
        throw std::runtime_error("No listen directive found.");
    for (size_t i = pos + 7 ; i < this->content.length(); i++)
    {
        if (content[i] == ';')
            break;
        port += content[i];
    }
    return toInt(port);

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