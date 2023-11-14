#include "configParsing.hpp"

int toInt(std::string str)
{
    int tmp = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == ' ' || str[i] == '\t')
            continue;
        if ((str[i] == '\'' || str[i] == '\"') && isdigit(str[i + 1]))
            continue;
        if (isdigit(str[i]))
            tmp = tmp * 10 + (str[i] - '0');
        else
            throw std::runtime_error("Port is not a number.");
    }
    return tmp;
}

char *ConfigParser::getFileName(const char *extension)
{
    char *fileName = NULL;
    int found = 0;

    dir = opendir(".");
    if (dir != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_REG && strstr(ent->d_name, extension) != NULL)
            {
                fileName = ent->d_name;
                found++;
                break;
            }
        }
        if (found > 1)
            throw std::runtime_error("More than one .conf file found in the directory.");
    }
    return fileName;
}

std::list<std::string> split(std::string &str, std::string delimiter)
{
    std::list<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        if (end != start)
        {
            tokens.push_back(str.substr(start, end - start));
        }
        start = end + delimiter.length();
    }

    if (start < str.length())
    {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}

bool isLast(std::string str, char delimiter)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == ' ' || str[i] == '\t')
            continue;
        if (str[i] == delimiter && str[i + 1] == '\0')
            return true;
    }
    return false;
}

bool notIn(std::string str, std::string s)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        if (s.find(str[i]) == std::string::npos)
            return true;
    }
    return false;
}

void ereaseContent(std::string &content, size_t pos, char dekimiter)
{
    if (content.find(dekimiter) == std::string::npos)
        throw std::runtime_error("No closing bracket found in server directive.");
    for (size_t i = pos; i < content.length(); i++)
    {
        if (content[i] == dekimiter)
        {
            content.erase(i--, 1);
            break;
        }
        content.erase(i--, 1);
    }
}

bool findFile(std::string path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
        return true;
    else 
        return false;
}

bool ifClosed(std::string line)
{
    for (size_t i = 0; i < line.length(); i++)
    {
        if(line[i] == ';' && line[i + 1] == '\0')
            return true;
    }
    return false;
}

bool ifCgi(std::string location)
{
    std::string locationName = "";
    size_t start = location.find("location");
    for (size_t i = start + 8; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == '(' || location[i] == '\n')
            break;
        locationName += location[i];
    }
    if (locationName.find(".") != std::string::npos)
        return true;
    return false;
}