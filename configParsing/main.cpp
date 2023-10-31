#include "configParsing.hpp"

int main(int argc, const char **argv)
{
    try
    {
        ConfigParser cfp(argv);
        cfp.readConfigFile();
        cfp.checkBrackets();
        std::cout << cfp.getPort() << std::endl;

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}