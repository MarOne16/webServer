#include "configParsing.hpp"

void ConfigParser::setConfKeys()
{
    confKeys.push_back("server");
    confKeys.push_back("listen");
    confKeys.push_back("location");
    confKeys.push_back("error_page");
    confKeys.push_back("alias");
    confKeys.push_back("root");
    confKeys.push_back("index");
    confKeys.push_back("cgi_path");
    confKeys.push_back("autoindex");
    confKeys.push_back("fastcgi_pass");
    confKeys.push_back("cgi_extension");
    confKeys.push_back("allow");
    confKeys.push_back("return");
    confKeys.push_back("upload_store_directory");
    confKeys.push_back("upload_enable");
    confKeys.push_back("server_name");
    confKeys.push_back("host");
    confKeys.push_back("port");
    confKeys.push_back("max_body_size");
    confKeys.push_back("default_location");
    confKeys.push_back("request_content");
    confKeys.push_back("}");
    confKeys.push_back("{");
    // confKeys.push_back(";");
    confKeys.push_back(")");
    confKeys.push_back("(");
    confKeys.push_back("client_max_body_size");
    confKeys.push_back("client_body_buffer_size");
}

std::string getKey(std::string line)
{
    std::string key;
    std::list<std::string> keys;
    keys = split(line, " ");
    key = keys.begin()->c_str();
    return key;
}

void ConfigParser::check_if_in_confKeys(std::string key)
{
    if (key.find("index.") != std::string::npos)
        return ;
    std::list<std::string>::iterator it = confKeys.begin();
    while (it != confKeys.end())
    {
        if (key == *it)
            return ;
        it++;
    }
    throw std::runtime_error("Error: " + key + " is not a valid key");
}
