#include "../Response/webserver.hpp"

int getServerId(std::map<unsigned int, server> &serv, int port, std::string server_name, std::string host)
{
    (void)host;
    std::map<unsigned int, server>::iterator it = serv.begin();
    std::cout  << port << std::endl;
    while (it != serv.end())
    {
        if ((it->second.port == port) && (it->second.server_name.find(server_name) != std::string::npos))
            return (it->first);
        it++;
    }
    return (0);
}

 


