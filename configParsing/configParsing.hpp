#pragma once

//colors
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */


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
#include <unistd.h>

typedef struct s_location
{
    std::string                         root;
    std::string                         alias;
    std::string                         index;
    std::string                         cgi_path;
    std::string                         autoindex;
    std::string                         fastcgi_pass;
    std::string                         cgi_extension;
    std::string                         allowed_methods;
    std::string                         return_code_url;
    std::string                         upload_store_directory;
    std::string                         upload_enable;
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
    bool connection;
} server;

class ConfigParser
{
    private:
        DIR *dir;           // for reading directory
        struct dirent *ent; // for reading directory
        char *m_fileName;
        std::list<std::string> confKeys;
        ConfigParser();

    public:
        std::string global_root;
        std::string global_upload_store;
        std::map<std::string, location> m_locations;
        std::map<unsigned int , server> m_servers;
        std::string content;
        std::string servers_content; // content of one server
        // methods for parsing
        void setConfKeys();
        void check_if_in_confKeys(std::string line);
        char *getFileName(const char *extension);
        bool isValideScope(std::string scope);
        bool ifInside(std::string scope, std::string toFind);
        void readConfigFile();
        void checkBrackets();
        void feedContent();
        bool ifOutsideLocation(std::string line);
        void globalUpload();

        // geters
        int                         getPort();
        unsigned int                getNumber_ofServers();
        std::string                 getServerName();
        std::string                 getRootServ();
        std::string                 getHost();
        std::string                 getMaxBodySize();
        std::map<std::string, std::string> getErrorPages();
        void feedLocations(); // feed m_locations
        bool isInsidLocation(std::string location ,std::string toFind);
        std::string getLocationName(std::string location);
        std::string getRootLocation(std::string location); // get root from location
        std::string getAlias(std::string location); // get alias from location
        std::string getIndex(std::string location); // get index from location
        std::string getCgiPath(std::string location); // get cgi_path from location
        std::string getAutoindex(std::string location); // get autoindex from location
        std::string getFastcgiPass(std::string location); // get fastcgi_pass from location
        std::string getCgiExtension(std::string location); // get cgi_extension from location
        std::string getAllowedMethods(std::string location); // get allowed_methods from location
        std::string getReturnCodeUrl(std::string location); // get return_code_url from location
        std::string getUploadEnable(std::string location); // get upload_enable from location
        std::string getUploadDirectory(std::string location);
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
bool ifClosed(std::string line);
bool ifCgi(std::string location);
bool notHas(std::string str, std::string s);
void addElement(std::string &str, std::string element, size_t pos);
void checkServer(std::map<unsigned int , server> &m_servers);
std::string getDefault(std::string path);
std::string getKey(std::string line);
