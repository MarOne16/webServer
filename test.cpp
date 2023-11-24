#include <iostream>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

<<<<<<< HEAD
int remove_all_files(const char *dirname)
{
    struct dirent* entity;
     DIR *dir = opendir(dirname);
     std::string filename = dirname;
    
    if(dir == NULL)
        return -1;
    entity = readdir(dir);
    while(entity != NULL)
=======
std::string get_Content_type(std::string url)
{


    std::string contentTypes[23] = {
        "text/plain",
        "text/html",
        "text/css",
        "text/javascript",
        "text/xml",
        "application/json",
        "application/xml",
        "application/xhtml+xml",
        "application/pdf",
        "application/octet-stream",
        "image/jpeg",
        "image/png",
        "image/gif",
        "image/svg+xml",
        "audio/mpeg",
        "audio/wav",
        "video/mp4",
        "video/webm",
        "multipart/form-data",
        "multipart/byteranges",
        "application/x-www-form-urlencoded",
        "application/graphql",
        "application/vnd.api+json",
    };
    std::string extension = url.substr(url.rfind(".") + 1);
 
	// if (_request.get_header_value("Content-Type:").size())
	// 	return _request.get_header_value("Content-Type:");
// 	if (extension.compare("html") == 0 || extension.compare("php") == 0)
// 		return "text/html; charset=UTF-8";
// 	else if (extension.compare("json") == 0)
// 		return "application/json";
// 	else if (extension.compare("ico") == 0)
// 		return "image/x-icon";
// 	else if (extension.compare("jpeg") == 0)
// 		return "image/jpeg";
// 	else if (extension.compare("jpg") == 0)
// 		return "image/jpg";
// 	else if (extension.compare("txt") == 0)
// 		return "text/plain";
//    else if (extension.compare("mp4") == 0)
//         return "video/mp4";
// std::cout  << extension << std::endl;
unsigned int i = 0;
while(i < 23)
{
        std::cout  << "gere" << std::endl;
    if(contentTypes[i].find(extension) != std::string::npos)
>>>>>>> 40e0c7843912740d2053bb67bbb52c425823b4c7
    {
        
        
        if(entity->d_name[0] != '.')
        {
            filename += entity->d_name;
            std::cout << filename <<"|" << std::endl;
            if(access(filename.c_str(), F_OK | W_OK) == -1)
            {
                return 1;
            }
            remove(filename.c_str());

        }
        entity = readdir(dir);
    }
    closedir(dir);
    return 0;
    }

int main()
{
<<<<<<< HEAD
    std::cout << "type: |" << remove_all_files("./src/test/") << std::endl;
=======
    std::cout  << "type:" << get_Content_type("img.mp4") << std::endl;
>>>>>>> 40e0c7843912740d2053bb67bbb52c425823b4c7
}