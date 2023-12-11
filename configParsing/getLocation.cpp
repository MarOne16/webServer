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

std::string ConfigParser::getRootLocation(std::string location)
{
    if (!isInsidLocation(location, "root"))
    {
        return this->global_root;
    }
    std::string root = "";
    size_t start = location.find("root");
    for (size_t i = start + 4; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                root += location[i];
            break;
        }
        root += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(root))
        throw std::runtime_error("Root Location directive is not closed.");
    return root.erase(root.length() - 1, 1);
}

std::string ConfigParser::getAlias(std::string location)
{
    if (!isInsidLocation(location, "alias"))
        return "";
    std::string alias = "";
    size_t start = location.find("alias");
    for (size_t i = start + 5; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                alias += location[i];
            break;
        }
        alias += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(alias))
        throw std::runtime_error("Alias directive is not closed.");
    return alias.erase(alias.length() - 1, 1);
}

std::string ConfigParser::getIndex(std::string location)
{
    if (!isInsidLocation(location, "tryfile"))
        return "";
    std::string index;
    size_t start = location.find("tryfile");
    for (size_t i = start + 7; i < location.length(); i++)
    {
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                index += location[i];
            break;
        }
        index += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(index))
        throw std::runtime_error("Index directive is not closed.");
    index.erase(index.length() - 1, 1);
    return index;
}

std::string ConfigParser::getCgiPath(std::string location)
{
    std::string cgi_extension = getCgiExtension(location);
    if (cgi_extension == "")
        return "";
    std::string cgi_path = "";
    size_t start = location.find("cgi_path");
    for (size_t i = start + 8; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                cgi_path += location[i];
            break;
        }
        cgi_path += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(cgi_path))
        throw std::runtime_error("Cgi path directive is not closed.");
    cgi_path.erase(cgi_path.length() - 1, 1);
    if (findFile(cgi_path) == false)
        throw std::runtime_error("Cgi path is not valid.");
    return cgi_path;
}

std::string ConfigParser::getAutoindex(std::string location)
{
    if (!isInsidLocation(location, "autoindex"))
        return "on";
    std::string autoindex = "";
    size_t start = location.find("autoindex");
    for (size_t i = start + 9; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                autoindex += location[i];
            break;
        }
        autoindex += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(autoindex))
        throw std::runtime_error("Autoindex directive is not closed.");
    autoindex.erase(autoindex.length() - 1, 1);
    if (autoindex != "on" && autoindex != "off" && autoindex != "ON" && autoindex != "OFF")
        throw std::runtime_error("autoindex must be on or off.");
    return autoindex;
}

std::string ConfigParser::getCgiExtension(std::string location)
{
    if (!isInsidLocation(location, "cgi_extension") || !ifCgi(location))
        return "";
    std::string cgi_extension = "";
    size_t start = location.find("cgi_extension");
    for (size_t i = start + 13; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                cgi_extension += location[i];
            break;
        }
        cgi_extension += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(cgi_extension))
        throw std::runtime_error("Cgi extension directive is not closed.");
    cgi_extension.erase(cgi_extension.length() - 1, 1);
    return cgi_extension;
}


std::string ConfigParser::getFastcgiPass(std::string location)
{
    if (!ifCgi(location) || !isInsidLocation(location, "fastcgi_pass"))
        return "";
    std::string fastcgi_pass = "";
    size_t start = location.find("fastcgi_pass");
    for (size_t i = start + 12; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                fastcgi_pass += location[i];
            break;
        }
        fastcgi_pass += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(fastcgi_pass))
        throw std::runtime_error("Fastcgi pass directive is not closed.");
    fastcgi_pass.erase(fastcgi_pass.length() - 1, 1);
    return fastcgi_pass;
}

std::string ConfigParser::getAllowedMethods(std::string location)
{
    if (!isInsidLocation(location, "allow"))
        return "GET";
    std::string allowed_methods = "";
    size_t start = location.find("allow");
    for (size_t i = start + 5; i < location.length(); i++)
    {
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                allowed_methods += location[i];
            break;
        }
        allowed_methods += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(allowed_methods))
        throw std::runtime_error("Allowed methods directive is not closed.");
    allowed_methods.erase(allowed_methods.length() - 1, 1);
    return allowed_methods;
}

std::string ConfigParser::getReturnCodeUrl(std::string location)
{
    if (!isInsidLocation(location, "return"))
        return "";
    std::string return_code_url  = "";
    size_t start = location.find("return");
    for (size_t i = start + 6; i < location.length(); i++)
    {
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                return_code_url += location[i];
            break;
        }
        return_code_url += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(return_code_url))
        throw std::runtime_error("Return code url directive is not closed.");
    return_code_url.erase(return_code_url.length() - 1, 1);
    std::list<std::string> list = split(return_code_url, " ");
    if (list.size() != 2)
        throw std::runtime_error("Return code url directive is not valid.");
    if (list.begin()->length() != 3 || notIn(*list.begin(), "0123456789"))
        throw std::runtime_error("Return code url directive is not valid.");
    return return_code_url;
}

std::string ConfigParser::getUploadDirectory(std::string location)
{
    std::string upload_directory = "";
    size_t start = location.find("upload_store");
    if (start == std::string::npos)
        return global_upload_store;
    for (size_t i = start + 12; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                upload_directory += location[i];
            break;
        }
        upload_directory += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(upload_directory))
        throw std::runtime_error("Upload directory directive is not closed.");
    upload_directory.erase(upload_directory.length() - 1, 1);
    return upload_directory;
}

std::string ConfigParser::getLocationName(std::string location)
{
    addElement(location, " ", location.find("("));
    std::string firstline = location.substr(0, location.find('\n'));
    std::list<std::string> list = split(firstline, " ");
    if (list.size() == 2)
    {
        std::list<std::string>::iterator it = list.begin();
        if (*it != "location")
            throw std::runtime_error("Location directive is not valid.");
        it++;
        if (notHas(*it, " )\t\n("))
            return *it;
        else
            throw std::runtime_error("Location directive is not valid.");
    }
    if (list.size() == 3)
    {
        std::list<std::string>::iterator it = list.begin();
        if (*it != "location")
            throw std::runtime_error("Location directive is not valid.");
        it++;
        if (notHas(*it, " )\t\n("))
            return *it;
        else
        {
            it++;
            if (*it != "(" || *it != "\n" || *it != "\t")
                throw std::runtime_error("Location directive is not valid.");
            return *--it;
        }
    }
    throw std::runtime_error("Location directive is not valid.");
}

std::string ConfigParser::getUploadEnable(std::string location)
{
    if (!isInsidLocation(location, "upload_enable"))
        return "off";
    std::string upload_enable = "";
    size_t start = location.find("upload_enable");
    for (size_t i = start + 13; i < location.length(); i++)
    {
        if (location[i] == ' ' || location[i] == '\t')
            continue;
        if (location[i] == ';' || location[i] == '\n')
        {
            if (location[i] == ';')
                upload_enable += location[i];
            break;
        }
        upload_enable += location[i];
        location.erase(i--, 1);
    }
    if (!ifClosed(upload_enable))
        throw std::runtime_error("Upload enable directive is not closed.");
    upload_enable.erase(upload_enable.length() - 1, 1);
    if (upload_enable != "on" && upload_enable != "off" && upload_enable != "ON" && upload_enable != "OFF")
        throw std::runtime_error("upload_enable must be on or off.");
    return upload_enable;
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
    std::string locationname = getLocationName(this->content.substr(start, end));
    for (size_t i = start2 ; i < end; i++)
    {
        tmp.root = getRootLocation(this->content.substr(start2, end));
        tmp.alias = getAlias(this->content.substr(start2, end));
        tmp.index = getIndex(this->content.substr(start2, end));
        tmp.cgi_path = getCgiPath(this->content.substr(start, end));
        tmp.autoindex = getAutoindex(this->content.substr(start2, end));
        tmp.fastcgi_pass = getFastcgiPass(this->content.substr(start, end));
        tmp.cgi_extension = getCgiExtension(this->content.substr(start, end));
        tmp.return_code_url = getReturnCodeUrl(this->content.substr(start2, end));
        tmp.allowed_methods = getAllowedMethods(this->content.substr(start2, end));
        tmp.upload_store_directory = getUploadDirectory(this->content.substr(start2, end));
        tmp.upload_enable = getUploadEnable(this->content.substr(start2, end));
    }
    this->m_locations[locationname] = tmp;
    ereaseContent(this->content, start, ')');
    goto start;
}
