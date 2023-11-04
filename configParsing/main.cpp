#include "configParsing.hpp"

int main(int argc, const char **argv)
{
    try
    {
        (void) argc;
        ConfigParser cfp(argv);
        cfp.readConfigFile();
        cfp.checkBrackets();
        std::cout << cfp.getPort() << std::endl;
        std::cout << cfp.getServerName() << std::endl;
        std::cout << cfp.getHost() << std::endl;
        std::cout << cfp.getMaxBodySize() << std::endl;
        std::map<std::string, std::string> errorPages = cfp.getErrorPages();
        for (std::map<std::string, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
            std::cout << it->first << " " << it->second << std::endl;
        cfp.feedLocations();
        for (std::map<std::string, location>::iterator it = cfp.m_locations.begin(); it != cfp.m_locations.end(); it++)
        {
            std::cout << "location: " << it->first << std::endl;
            std::cout << "root: " << it->second.root << std::endl;
            std::cout << "index: " << it->second.index << std::endl;
            std::cout << "cgi_path: " << it->second.cgi_path << std::endl;
            std::cout << "autoindex: " << it->second.autoindex << std::endl;
            std::cout << "cgi_extension: " << it->second.cgi_extension << std::endl;
            std::cout << "allowed_methods: " << it->second.allowed_methods << std::endl;
            std::cout << "return_code_url: " << it->second.return_code_url << std::endl;
            
        }

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}