#include "./webserver.hpp"

int main()
{
    std::string requese = "GET /hello.htm HTTP/1.1 User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
Host: www.tutorialspoint.com:asahs
Accept-Language: en-us
Accept-Encoding: gzip, deflate
Connection: Keep-Alive";

    Requese req("GET /hello.htm HTTP/1.1 \n User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT) ");
}