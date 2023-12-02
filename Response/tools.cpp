#include "./webserver.hpp"


std::vector<std::string> split_v(std::string &str, std::string delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        if (end != start)
        {
            tokens.push_back(str.substr(start, end - start));
        }
        start = end + delimiter.length();
    }

    if (start < str.length())
    {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}



std::string Response::get_Date()
{
    time_t now = time(0);
    std::stringstream str;

    char *dt = ctime(&now);
    str << dt;
    return str.str().substr(0, str.str().size() - 1);
}


std::string Response::check_index_file(std::string &url)
{
    DIR *dir = opendir(url.c_str());
    std::vector<std::string> files = split_v(this->response_items.location->index, " "); // change by value depends on location
    if (dir == NULL)
        return "";
    struct dirent *entity;
    unsigned int i = 0;
    while (i < files.size())
    {
        entity = readdir(dir);
        while (entity != NULL)
        {
            if (entity->d_name == files[i])
                return files[i];
            entity = readdir(dir);
        }
        i++;
    }
    closedir(dir);
    return "";
}


int Response::remove_all_files(const char *dirname)
{
    struct dirent *entity;
    DIR *dir = opendir(dirname);
    std::string filename;
    ;

    if (dir == NULL)
        return -1;
    entity = readdir(dir);
    while (entity != NULL)
    {

        filename = dirname;
        if (entity->d_name[0] != '.')
        {
            filename += entity->d_name;
            if (access(filename.c_str(), F_OK | W_OK) == -1)
            {
                return 1;
            }
            remove(filename.c_str());
        }
        entity = readdir(dir);
        filename.clear();
    }
    closedir(dir);
    return 0;
}


std::string Response::trim(std::string original)
{
    unsigned int begin_index = 0;
    unsigned int i = 0;
    if (original.size() == 0)
        return "";
    while (isblank(original[i]) != 0 && i <= original.size())
    {
        begin_index++;
        i++;
    }
    i = original.size() - 1;
    while (isblank(original[i]) != 0 && i != 0)
        i--;
    if (begin_index == original.size())
        return "";
    return original.substr(begin_index, i + 1);
}



std::string  parserbody(std::string reqbody)
{
    // std::string reqbody="7\r\nMozilla\r\n0\r\n11\r\nDeveloper Network\r\n0\r\n\r\n";

std::stringstream ss;
std::stringstream body;
ss << reqbody;

int pos = 0 ;
std::string token;
int hash = -1;
 while(pos != -1 && hash)
{
    pos = reqbody.find("\r\n");
    if(reqbody[0] == '\r' && reqbody[1] == '\n')
    {
        reqbody = reqbody.substr(2);
        break;
    }
    token = reqbody.substr(0, pos);
    hash = std::stoi(token, 0, 16);
    if(hash != 0)
    {
        // if(hash > reqbody.length())
        //     return "";
        token = reqbody.substr(pos + 2 , hash);
        reqbody = reqbody.substr(pos + hash + 4);
        body << token;
    }
  
} 
return body.str();

}