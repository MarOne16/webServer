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
    std::string fastcgi_pass;
    std::string cgi_extension;
    std::string allowed_methods;
    std::string return_code_url;
    std::string upload_store_directory;
} location;

typedef struct s_server
{
    std::string server_name;
    std::string host;
    int         port;
    std::string default_location;
    std::string max_body_size;
    std::map<std::string, std::string> error_pages;
    std::map<std::string, location> locations;
    std::string request_content;
} server;

class ConfigParser
{
    private:
        DIR *dir;           // for reading directory
        struct dirent *ent; // for reading directory
        char *m_fileName;
        ConfigParser();

    public:
        std::map<std::string, location> m_locations;
        std::map<unsigned int , server> m_servers;
        std::string content;
        std::string servers_content; // content of one server
        // methods for parsing
        char *getFileName(const char *extension);
        bool isValideScope(std::string scope);
        bool ifInside(std::string scope, std::string toFind);
        void readConfigFile();
        void checkBrackets();
        void feedContent();

        // geters
        int             getPort();
        unsigned int    getNumber_ofServers();
        std::string     getServerName();
        std::string     getHost();
        std::string     getMaxBodySize();
        std::map<std::string, std::string> getErrorPages();
        void feedLocations(); // feed m_locations
        bool isInsidLocation(std::string location ,std::string toFind);
        std::string getRoot(std::string location); // get root from location
        std::string getIndex(std::string location); // get index from location
        std::string getCgiPath(std::string location); // get cgi_path from location
        std::string getAutoindex(std::string location); // get autoindex from location
        std::string getFastcgiPass(std::string location); // get fastcgi_pass from location
        std::string getCgiExtension(std::string location); // get cgi_extension from location
        std::string getAllowedMethods(std::string location); // get allowed_methods from location
        std::string getReturnCodeUrl(std::string location); // get return_code_url from location
        void feedServers(); // feed m_servers
        ConfigParser(const char **argv);
        ~ConfigParser();
};

// tools
int toInt(std::string str);
std::list<std::string> split(std::string &str, std::string delimiter);
bool isLast(std::string str, char delimiter);
bool notIn(std::string str, std::string s);
void ereaseContent(std::string &content, size_t pos, char dekimiter);
bool findFile(std::string path);