#include "../Response/webserver.hpp"

int getServerId(std::map<unsigned int, server> &serv, int port, std::string server_name, std::string host)
{
    std::map<unsigned int, server>::iterator it = serv.begin();
    while (it != serv.end())
    {
        if ((it->second.port == port) && (it->second.server_name.find(server_name) != std::string::npos) && (it->second.host.find(host) != std::string::npos))
            return (it->first);
        it++;
    }
    it = serv.begin();
    while (it != serv.end())
    {
        if ((it->second.port == port) && (it->second.server_name.find(server_name) != std::string::npos))
            return (it->first);
        it++;
    }
    it = serv.begin();
    while (it != serv.end())
    {
        if (it->second.port == port && (it->second.host.find("0.0.0.0") != std::string::npos))
            return (it->first);
        it++;
    }
    it = serv.begin();
    while (it != serv.end())
    {
        if (it->second.port == port)
            return (it->first);
        it++;
    }
    return (0);
}
