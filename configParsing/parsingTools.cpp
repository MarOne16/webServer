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

char *getFileName(const char *extension)
{
    DIR *dir;
    struct dirent *ent;
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
        closedir(dir);
    }
    return fileName;
}

std::map<std::string, std::string> split(std::string str, char delimiter)
{
    std::map<std::string, std::string> map;
    std::string key;
    std::string value;
    std::istringstream iss(str);
    std::getline(iss, key, delimiter);
    std::getline(iss, value, delimiter);
    map[key] = value;
    return map;
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
