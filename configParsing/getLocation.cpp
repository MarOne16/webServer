#include "configParsing.hpp"

bool ConfigParser::isInsidLocation(std::string location ,std::string toFind)
{
    size_t start = location.find(toFind);
    size_t firstbracket = location.find('(');
    size_t lastbracket = location.find(')');
    if (start > firstbracket && start < lastbracket)
        return true;
    return false;
}

std::string ConfigParser::getRoot(std::string location)
{
    if (!isInsidLocation(location, "root"))
        throw std::runtime_error("Root path is not aviabile.");
    std::string root = "";
    size_t start = location.find("root");
    size_t end = location.find(';', start);
    root = location.substr(start + 5, end - start - 5);
    for (size_t i = 0; i < root.size(); i++)
    {
        if (root[i] == ' ')
        {
            root.erase(i, 1);
            i--;
        }
    }
    std::ifstream file(root.c_str());
    if (!file.good())
        throw std::runtime_error("Root path is not valid.");
    return root;
}

std::string ConfigParser::getIndex(std::string location)
{
    if (!isInsidLocation(location, "index"))
        return "";
    std::string index;
    size_t start = location.find("index");
    size_t end = location.find(';', start);
    index = location.substr(start + 6, end - start - 6);
    return index;
}

std::string ConfigParser::getCgiPath(std::string location)
{
    std::string cgi_extension = getCgiExtension(location);
    if (cgi_extension == "")
        return "";
    std::string cgi_path = "";
    size_t start = location.find("root");
    size_t end = location.find(';', start);
    cgi_path = location.substr(start + 5, end - start - 5);
    for (size_t i = 0; i < cgi_path.size(); i++)
    {
        if (cgi_path[i] == ' ')
        {
            cgi_path.erase(i, 1);
            i--;
        }
    }
    std::ifstream file(cgi_path.c_str());
    if (!file.good())
        throw std::runtime_error("Cgi path is not valid.");
    return cgi_path;
}

std::string ConfigParser::getAutoindex(std::string location)
{
    if (!isInsidLocation(location, "autoindex"))
        return "";
    std::string autoindex = "";
    size_t start = location.find("autoindex");
    size_t end = location.find(';', start);
    autoindex = location.substr(start + 10, end - start - 10);
    for (size_t i = 0; i < autoindex.size(); i++)
    {
        if (autoindex[i] == ' ')
        {
            autoindex.erase(i, 1);
            i--;
        }
    }
    if (autoindex != "on" && autoindex != "off")
        throw std::runtime_error("autoindex must be on or off.");
    return autoindex;
}

std::string ConfigParser::getCgiExtension(std::string location)
{
    std::string cgi_extension = "";
    for (std::string::iterator it = location.begin(); it != location.end(); it++)
    {
        if (*it == '.')
        {
            cgi_extension = "";
            while (*(it) != '(')
            {
                cgi_extension += *it;
                it++;
            }
            break;
        }
    }
    return cgi_extension;

}

std::string ConfigParser::getAllowedMethods(std::string location)
{
    std::string methods = "";
    start:
    if (!isInsidLocation(location, "allow"))
        return methods;
    std::string allowed_methods = "";
    size_t start = location.find("allow");
    size_t end = location.find(';', start);
    allowed_methods = location.substr(start + 5, end - start - 5);
    for (size_t i = 0; i < allowed_methods.size(); i++)
    {
        if (allowed_methods[i] == ' ')
        {
            allowed_methods.erase(i, 1);
            i--;
        }
    }
    if (allowed_methods != "GET" && allowed_methods != "POST" && allowed_methods != "DELETE")
        throw std::runtime_error("Allowed methods must be GET, POST or DELETE.");
    methods += allowed_methods;
    methods += " ";
    ereaseContent(location, start, ';');
    if (isInsidLocation(location, "allow"))
        goto start;
    return methods;
}

std::string ConfigParser::getReturnCodeUrl(std::string location)
{
    if (!isInsidLocation(location, "return_code_url"))
        return "";
    std::string return_code_url = "";
    size_t start = location.find("return_code_url");
    size_t end = location.find(';', start);
    return_code_url = location.substr(start + 16, end - start - 16);
    for (size_t i = 0; i < return_code_url.size(); i++)
    {
        if (return_code_url[i] == ' ')
        {
            return_code_url.erase(i, 1);
            i--;
        }
    }
    return return_code_url;
}

void ConfigParser::feedLocations()
{
    start:
    location tmp;
    if (!ifInside("server", "location"))
        return;
    size_t start = this->content.find("location");
    size_t start2 = this->content.find('(', start);
    size_t end = this->content.find(')', start);
    if (start2 == std::string::npos || end == std::string::npos)
        throw std::runtime_error("No closing bracket found in location directive.");
    std::string locationname = this->content.substr(start + 9, start2 - start - 10);

    for (size_t i = start2 ; i < end; i++)
    {
        tmp.root = getRoot(this->content.substr(start2, end));
        tmp.index = getIndex(this->content.substr(start2, end));
        tmp.cgi_path = getCgiPath(this->content.substr(start, end));
        tmp.autoindex = getAutoindex(this->content.substr(start2, end));
        tmp.cgi_extension = getCgiExtension(this->content.substr(start, end));
        tmp.allowed_methods = getAllowedMethods(this->content.substr(start2, end));
        tmp.return_code_url = getReturnCodeUrl(this->content.substr(start2, end));
    }
    this->m_locations.insert(std::pair<std::string, location>(locationname, tmp));
    ereaseContent(this->content, start, ')');
    goto start;
}
