#include <webserver.hpp>


std::vector<std::string> split(std::string &str, std::string delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        if (end != start)
        {
            tokens.push_back(str.substr(start, end - start));
        }
        start = end + delimiter.length();
    }

    if (start < str.length())
    {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}