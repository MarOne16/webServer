#include "configParsing.hpp"

int main(int argc, const char **argv)
{
    try
    {
        (void) argc;
        ConfigParser cfp(argv);
        cfp.readConfigFile();
        cfp.checkBrackets();
        checkServer(cfp.m_servers);
        std::map<unsigned int, server>::iterator it = cfp.m_servers.begin();
        while (it != cfp.m_servers.end())
        {
            std::cout  << "server_name: " << it->second.server_name << std::endl;
            std::cout  << "host: " << it->second.host << std::endl;
            std::cout  << "port: " << it->second.port << std::endl;
            std::cout  << "max_body_size: " << it->second.max_body_size << std::endl;
            std::cout  << "Default path: " << it->second.default_location << std::endl;
            std::cout  << "error_pages: " << std::endl;
            std::map<std::string, std::string>::iterator it2 = it->second.error_pages.begin();
            while (it2 != it->second.error_pages.end())
            {
                std::cout  << it2->first << ": " << it2->second << std::endl;
                it2++;
            }
            std::map<std::string, location>::iterator it3 = it->second.locations.begin();
            while (it3 != it->second.locations.end())
            {
                std::cout  << "locations: " << it3->first << std::endl;
                std::cout  << YELLOW"root: " << it3->second.root << std::endl;
                std::cout  << "index: " << it3->second.index << std::endl;
                std::cout  << "cgi_path: " << it3->second.cgi_path << std::endl;
                std::cout  << "autoindex: " << it3->second.autoindex << std::endl;
                std::cout  << "cgi_extension: " << it3->second.cgi_extension << std::endl;
                std::cout  << "fastcgi_pass: " << it3->second.fastcgi_pass << std::endl;
                std::cout  << "allowed_methods: " << it3->second.allowed_methods << std::endl;
                std::cout  << "return_code_url: " << it3->second.return_code_url << std::endl;
                std::cout  << "upload_store_directory: " << it3->second.upload_store_directory << RESET<< std::endl;
                it3++;
            }
            it++;
        }
        std::cout  << "the number of servers: " << cfp.getNumber_ofServers() << std::endl;

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}