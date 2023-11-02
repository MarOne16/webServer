#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
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

typedef struct s_location
{
    std::string root;
    std::string index;
    std::string cgi_path;
    std::string autoindex;
    std::string cgi_extension;
    std::string allowed_methods;
    std::string return_code_url;
    std::string upload_store_directory;
} location;

class ConfigParser
{
    private:
        DIR *dir;           // for reading directory
        struct dirent *ent; // for reading directory
        char *m_fileName;
        std::map<std::string, location> m_locations;
        ConfigParser();

    public:
        std::string content;
        // methods for parsing
        char *getFileName(const char *extension);
        bool isValideScope(std::string scope);
        bool ifInside(std::string scope, std::string toFind);
        void readConfigFile();
        void checkBrackets();

        // geters
        int getPort();
        std::string getServerName();
        std::string getHost();
        std::string getMaxBodySize();
        std::map<std::string, std::string> getErrorPages();
        void feedLocations();
        void feedConfMap();
        ConfigParser(const char **argv);
        ~ConfigParser();
};

// tools
int toInt(std::string str);
std::list<std::string> split(std::string &str, std::string delimiter);
bool isLast(std::string str, char delimiter);
bool notIn(std::string str, std::string s);
void ereaseContent(std::string &content, size_t pos);