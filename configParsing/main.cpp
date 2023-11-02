#include "configParsing.hpp"

int main(int argc, const char **argv)
{
    try
    {
        ConfigParser cfp(argv);
        cfp.readConfigFile();
        cfp.checkBrackets();
        std::cout << cfp.getPort() << std::endl;
        std::cout << cfp.getServerName() << std::endl;
        std::cout << cfp.getHost() << std::endl;
        std::cout << cfp.getMaxBodySize() << std::endl;
        std::map<std::string, std::string> errorPages = cfp.getErrorPages();
        std::map<std::string, std::string> errorPages2 = cfp.getErrorPages();
        for (std::map<std::string, std::string>::iterator it = errorPages2.begin(); it != errorPages2.end(); it++)
            std::cout << it->first << " " << it->second << std::endl;

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}