#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include "./servers.hpp"
#include "../Response/webserver.hpp"
#define PORT 8080


void feedRequest(unsigned int index, std::map<unsigned int , server> &serv, std::string content)
{
    std::map<unsigned int , server>::iterator it = serv.begin();
    while (it != serv.end())
    {
        if (it->first == index)
        {
            it->second.request_content = content;
            break;
        }
        it++;
    }
    
}
std::string  sendResponse(unsigned int index, std::map<unsigned int , server> &serv)
{
    std::map<unsigned int , server>::iterator it = serv.begin();
    while (it != serv.end())
    {
        if (it->first == index)
            return(Get_response(it->second));
        it++;
    }
    return ("");
}

int main(int argc, const char **av)
{
    argc = 0;
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
      /////////////////////////////
    ConfigParser data_conf(av);//
    data_conf.readConfigFile();//
    data_conf.checkBrackets();///
    /////////////////////////////
    // Only this line has been changed. Everything is same.
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( data_conf.getPort() );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
        feedRequest(0, data_conf.m_servers, buffer);
        std::string respense =  sendResponse(0, data_conf.m_servers);
        std::cout  << respense << std::endl;
        ///////////////////////////////////////////////
        send(new_socket,respense.c_str(), respense.length(), 0);
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}