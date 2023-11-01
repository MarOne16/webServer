#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include "./webserver.hpp"


int main()
{
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if(listening == -1 )
    {
        std::cerr << "not listennig " << std::endl;
        return -1;
    }
    sockaddr_in uint;
    uint.sin_family = AF_INET;
    uint.sin_port = htons(5400);
    inet_pton(AF_INET, "0.0.0.0", &uint.sin_addr);
    if(bind(listening, (sockaddr*)&uint, sizeof(uint)) == - 1)
    {
        std::cerr << "not bind" << std::endl;
        return -1;
    }
    if(listen(listening, SOMAXCONN) == -1)
    {
        std::cerr << "not listen" << std::endl;
        return -1;
    }
    std::cout << "bind sure" << std::endl;

}