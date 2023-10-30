#include "configParsing.hpp"

int main(int argc, const char **argv)
{
    try
    {
        ConfigParser cfp(argv);
        cfp.readConfigFile();
        std::cout << cfp.content << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}