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
    std::map<std::string, std::string> config_map;
    ConfigParser();
    public:
    std::string content;
    bool isValideScope(std::string scope);
    bool ifInside(std::string scope, std::string toFind);
    void readConfigFile();
    void feedConfMap();
    void checkBrackets();
    int  getPort();
    ConfigParser(const char **argv);
    ~ConfigParser();
};
int toInt(std::string str);
char *getFileName(const char *extension);
std::map<std::string, std::string> split(std::string str, char delimiter);
bool isLast(std::string str, char delimiter);
// bool checkBrackets(const std::string &str);