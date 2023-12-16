#pragma once

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sock=addr_in
#include <cstdlib>      // For exit() and EXIT_FAILURE     // For cout
#include <unistd.h>     // For read
#include <poll.h>
#include <fcntl.h>
#include <fstream>
#include "../configParsing/configParsing.hpp"
#include "../Response/webserver.hpp"
#include <arpa/inet.h>


class Servers
{
    public : 
        std::vector<int> serveur;
        std::map<int, std::string> num;
        std::map<int, size_t> flags;
        std::map<int, std::string> number;
        std::vector<struct sockaddr_in> addresses;
        std::vector<socklen_t> addresselent;
        std::vector<int> new_cont;
        std::string respense;
        std::map<int, std::string> map_request;
        std::map<int, size_t> checker;
        std::map<int, std::string> response_map;
        std::string request;
        std::vector<int> client;
        std::map<int, int> chunked;
        std::map<int, size_t> len_requeste;
        std::map<int, bool> connection;
        std::vector<struct pollfd> fds;
        std::map<int , size_t > len_read ;


};
void feedRequest(unsigned int index, std::map<unsigned int, server> &serv, std::string content);
int is_Host(std::string host);
void ports(std::vector<int> &port, std::map<unsigned int, server> &data_serv);
std::string sendResponse(unsigned int index, std::map<unsigned int, server> &serv);
std::string inforamation(std::string reqeust, size_t i);
int is_digit(char c);
void ignore_espace(std::string &name);
void port_name_serveur(std::string request, std::string &port, std::string &name_serveur);
void geve_port_host(std::string request, std::string &name_serveur, std::string &port);
void geve_port_serveur(std::string request, std::string &name_serveur);
void replaceAll(std::string &str, const std::string &from, const std::string &to);
void create_soket(ConfigParser data_conf, Servers & serveur);
void add_serveur(std::vector<struct pollfd> &fds, std::vector<int> serveur);
int read_to_client( Servers &sereur , int i,ConfigParser data_conf);
void follow_responsive(int &cheker, int fd,  Servers  &serveur );
void accepte_client(int fd, std::vector<int> &client, std::vector<struct pollfd> &fds);
void close_fd(int fd,Servers & serveur   );
void delete_maps(std::map<int, std::string> &request, std::map<int, std::string> &responce, std::map<int, size_t> &lent_response, std::map<int, int> &is_chunked, std::map<int, size_t> &cheker, int fd);
void request_inserer(char *buffer, int buff_size, int fd,Servers &serveur  , int &stop    );
size_t content_lenght(std::string request);
size_t length_heder(std::string request);
std::string data(std::map<int, std::string> map_request, int fd);