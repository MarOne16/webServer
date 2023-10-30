#pragma once 

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>


class ConfigParser
{
    private:
        char  *m_fileName;
        ConfigParser();
    public:
        std::string content;
        void readConfigFile();
        ConfigParser(const char **argv);
        ~ConfigParser();
};
char *getFileName(const char *extension);
// bool checkBrackets(const std::string &str);