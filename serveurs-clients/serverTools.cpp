#include "../Response/webserver.hpp"

int getServerId(std::map<unsigned int, server> &serv, int port, std::string server_name)
{
    std::map<unsigned int, server>::iterator it = serv.begin();
    while (it != serv.end())
    {
        if (it->second.port == port && it->second.server_name == server_name)
            return (it->first);
        it++;
    }
    return (-1);
}