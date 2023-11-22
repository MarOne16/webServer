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

int remove_all_files(const char *dirname)
{
    struct dirent* entity;
     DIR *dir = opendir(dirname);
     std::string filename = dirname;
    
    if(dir == NULL)
        return -1;
    entity = readdir(dir);
    while(entity != NULL)
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
    std::cout << "type: |" << remove_all_files("./src/test/") << std::endl;
}