#include "configParsing.hpp"

void ConfigParser::feedLocations()
{
    location *tmp = new location;
    if (!ifInside(this->content, "location"))
        return;
    size_t start = this->content.find("location");
    size_t end = this->content.find('}', start);
    for 
}
