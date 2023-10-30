#include "configParsing.hpp"

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
        this->content += line;
        this->content += '\n';
    }
}