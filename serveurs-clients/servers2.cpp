// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   main.cpp                                           :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: iedderqi <iedderqi@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2023/10/26 14:23:56 by iedderqi          #+#    #+#             */
/*   Updated: 2023/11/04 16:27:46 by iedderqi         ###   ########.fr          */
// /*                                                                            */
// /* ************************************************************************** */

// 1.1.7.1.3.5.8.3.8
// posst
// send
// chenck
#include "../Response/webserver.hpp"
#include <fstream>

/// multi serveurs whit multi clients
// TODO
/// service_name
/// chankese request
/// merege requset whit serveur

void feedRequest(unsigned int index, std::map<unsigned int, server> &serv, std::string content)
{
    std::map<unsigned int, server>::iterator it = serv.begin();
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
std::string sendResponse(unsigned int index, std::map<unsigned int, server> &serv)
{
    std::map<unsigned int, server>::iterator it = serv.begin();
    while (it != serv.end())
    {
        if (it->first == index)
            return (Get_response(it->second));
        it++;
    }
    return ("");
}
/// port----
void ports(std::vector<int> &port, std::map<unsigned int, server> &data_serv)
{
    std::map<unsigned int, server>::iterator itb = data_serv.begin();
    std::map<unsigned int, server>::iterator ite = data_serv.end();
    while (itb != ite)
    {
        port.push_back((itb->second.port));
        // std::cout  << (itb->second.port) << std::endl;
        itb++;
    }
}
int is_Host(std::string host)
{
    return (host == "Host");
}
std::string inforamation(std::string reqeust, size_t i)
{
    std::string data;

    for (size_t j = i; j < reqeust.size(); j++)
    {

        // std::
        if (reqeust[j] == '\n' || reqeust[j] == '\r')
            break;

        data = data + reqeust[j];
    }

    return (data);
}
int is_digit(char c)
{
    return (c >= '0' && c <= '9');
}
void ignore_espace(std::string &name)
{

    std::string name_serveur;

    for (size_t i = 0; i < name.size(); i++)
    {
        if (name[i] == ' ')
            break;
        name_serveur += name[i];
    }
    name = name_serveur;
}
void port_name_serveur(std::string request, std::string &port, std::string &name_serveur)
{
    size_t i;

    for (i = 1; i < request.size(); i++)
    {
        if (request[i] == ':')
            break;
        name_serveur += request[i];
    }
    ignore_espace(name_serveur);
    while (request[i] == ' ')
        i++;
    i++;
    for (size_t j = i; j < request.size(); j++)
    {
        if (!is_digit(request[j]))
            break;
        port += request[j];
    }
}
void geve_port_name(std::string request, std::string &name_serveur, std::string &port)
{

    for (size_t i = 0; i < request.size(); i++)
    {
        if (request[i] == '\r')
            i++;
        if (request[i] == '\n')
            i++;
        if (i + 4 < request.size() && is_Host(request.substr(i, 4)))

            port_name_serveur(inforamation(request, i + 5), port, name_serveur);
    }
}

void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

size_t length_heder(std::string request)
{

    for (size_t i = 0; i < request.size(); i++)
    {
        if (request[i] == '\r' || request[i] == '\n')
            i++;

        else if (i + 14 < request.size())
        {
            std::string chceck = request.substr(i, 14);

            if (chceck == "Content-Length")
            {
                i += 14;
                std::string number;
                i += 2;

                while (is_digit(request[i]))
                {

                    number += request[i];
                    i++;
                }

                std::stringstream stream(number);
                size_t output;

                stream >> output;

                return (output);
            }
        }
    }
    return (0);
}
size_t content_lenght(std::string request)
{
    std::string calcule;

    for (size_t i = 0; i < request.size(); i++)
    {
        //\r\n\r\n
        if (i + 3 < request.size() && request[i] == '\r' && request[i + 1] == '\n' && request[i + 2] == '\r' && request[i + 3] == '\n')
        {
            calcule += request[i];
            i++;
            calcule += request[i];
            i++;
            calcule += request[i];
            i++;
            calcule += request[i];
            i++;
            break;
        }
        calcule += request[i];
    }

    return (calcule.size());
}

size_t donner_flags(std::map<int, size_t> checker, int fd)
{
    std::map<int, size_t>::iterator it = checker.begin();
    for (it = checker.begin(); it != checker.end(); it++)
    {
        if (it->first == fd)
        {
            return (it->second);
        }
    }
    return (0);
}
std::string data(std::map<int, std::string> map_request, int fd)
{
    std::map<int, std::string>::iterator it = map_request.begin();
    for (it = map_request.begin(); it != map_request.end(); it++)
    {
        if (it->first == fd)
        {
            return (it->second);
        }
    }
    return ("");
}

// void request_inserer(char buffer[1024], int fd, std::map<int, std::string> &map_request, std::map<int, size_t> &checker)
// {

//      if (map_request.find(fd) == map_request.end())
//     {

//        map_request.insert(std::make_pair(fd, ""));
// checker.insert(std::make_pair(fd, 1337));

//     }
//     std::map<int, std::string>::iterator it;
//     std::map<int, size_t>::iterator it_checker = checker.begin();
//     for (it = map_request.begin(); it != map_request.end(); it++)
//     {
//         if (it->first == fd && it_checker->first == fd)
//         {
//             std::string co  = it->second;
//             std::string str = co;
//             std::string name = co;
//             str = name + buffer;
//             // int flage = fd ;
// std::cout <<fd;
//             map_request.erase(fd);

//            map_request.insert(std::make_pair(fd, str));

//             it_checker->second = content_lenght(name) + length_heder(name.substr(0, content_lenght(name)));

//              str.clear();
//              name.c_str();
//              co.c_str();
//             // exit(0);
//             return;
//         }
//         it_checker++;
//     }
// }
int index_fds( std::vector<struct pollfd> fds  , int fd)
{
    int i = 0;
    std::vector<struct pollfd>::iterator it  ;

    
    for(it =fds.begin() ; it != fds.end() ; it++ )
    {
        
        if(it->fd == fd )
            return( i );
        
        i++;

    }
    return(-1);
    
}
void request_inserer(char buffer[1024], int buff_size, int fd, std::map<int, std::string> &map_request, std::map<int, size_t> &checker)
{
    int j = 0;
    if (map_request.find(fd) == map_request.end())
    {
        map_request.insert(std::make_pair(fd, ""));
        checker.insert(std::make_pair(fd, 0));
        j = 1;
    }

    std::map<int, std::string>::iterator it = map_request.find(fd);
    std::map<int, size_t>::iterator it_checker = checker.find(fd);

    if (it != map_request.end() && it_checker != checker.end())
    {
        std::string first_request = std::string(buffer, buff_size);
        int lenght = it_checker->second;
        // coment_string () {}{ }{}{ }
        //  std::string name = it->second;
        //  std::string str = name + std::string(buffer, buff_size);

        // map_request.erase(fd);
        // map_request.insert(std::make_pair(fd, str));
        std::string request = std::string(buffer, buff_size);
        ;
        it->second += request;

        if (j == 1)
        {

            it_checker->second = content_lenght(first_request) + length_heder(first_request.substr(0, content_lenght(first_request)));
        }

        else
            it_checker->second = lenght;

        first_request.clear();
        request.clear();

        return;
    }
}

int main(int ac, const char **av)
{
    // how to serveu run the port serveur
    // create multi serveur
    //  innsilit port
    (void)ac;
    try
    {
        /////////////////////////////
        ConfigParser data_conf(av);       //
        data_conf.readConfigFile();       //
        data_conf.checkBrackets();        ///
        checkServer(data_conf.m_servers); ///
        /////////////////////////////
        std::vector<int> port;
        std::vector<int> file;

        std::vector<struct sockaddr_in> addresses;
        std::vector<socklen_t> addresselent;
        std::vector<int> new_cont;
        std::string respense;
        std::map<int, std::string> map_request;
        std::map<int, size_t> checker;
        ports(port, data_conf.m_servers);
        for (unsigned int i = 0; i < data_conf.getNumber_ofServers(); i++)
        {
            struct sockaddr_in adrese;
            socklen_t addrlen = sizeof(adrese);
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd < 0)
            {
                perror("socket:felaid ");
                exit(0);
            }
            int opt = 1;
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
            {
                perror("setsockopt");
            }
            if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(int)) == -1)
            {
                perror("setsockopt");
            }
            fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC); /// non-blocking file descriptors
            adrese.sin_addr.s_addr = INADDR_ANY;
            adrese.sin_family = AF_INET;
            adrese.sin_port = htons(port[i]);
            if (bind(fd, (struct sockaddr *)&adrese, sizeof(adrese)) < 0)
            {
                perror("bind Failed :");
                exit(0);
            }
            if (listen(fd, 3) < 0)
            {
                perror("listen Failed : ");
                exit(0);
            }
            file.push_back(fd);
            addresses.push_back(adrese);
            addresselent.push_back(addrlen);
        }
        std::vector<struct pollfd> fds;
        for (size_t i = 0; i < file.size(); i++)
        {
            struct pollfd poll;
            poll.fd = file[i];
            poll.events = POLLIN;
            fds.push_back(poll);
        }

        std::string request;

        std::vector<int> client;

        char buf[1024];
        size_t cherk;
        while (true)
        {
            int ret = poll(fds.data(), fds.size(), 0);
            if (ret == -1)
            {
                perror("POOLLL failed :");
                return (0);
            }

            for (size_t i = 0; i < fds.size(); i++)
            {
                // int lenght ;
                if (fds[i].revents & POLLIN)
                {

                    // if(std::find(client.begin(), file.end(), fds[i].fd) != file.end())
                    // {

                    // }
                    // std::cout<<fds[i].fd << "{}{}{}\n";

                    if (std::find(file.begin(), file.end(), fds[i].fd) != file.end())
                    {
                        std::cout << "serveur  \n";
                        int co = accept(fds[i].fd, NULL, NULL);
                        fcntl(co, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

                        if (co < 0)
                        {
                            perror("Faild ::  acceot");
                        }
                        else
                        {
                            struct pollfd fl;
                            fl.fd = co;
                            fl.events = POLLIN;
                            client.push_back(co);
                            fds.push_back(fl);
                        }
                    }
                    else if (!client.empty() || std::find(client.begin(), client.end(), fds[i].fd) != client.end())
                    {
                        // client bye bye

                        bzero(buf, 1024);
                        int rec = recv(fds[i].fd, buf, 1024, 0);

                        if (rec < 0)
                        {

                            perror("recv");
                            exit(1);
                        }
                        if (rec == 0)
                        {
                            close(fds[i].fd);
                            map_request.erase(fds[i].fd);
                            checker.erase(fds[i].fd);
                            fds.erase(fds.begin() + index_fds( fds  , fds[i].fd));
                            client.erase(std::find(client.begin(), client.end(), fds[i].fd));
                            std::cout << "bybye" << std::endl;
                        }
                        // request += std::string(buf, rec);
                        // if (flag == 0)
                        // cherk = content_lenght(request) + lenght_heder(request.substr(0, content_lenght(request)));
                        // if(  std::string(buf, rec).size() <= 0)
                        //     exit(0);

                        request_inserer(buf, rec, fds[i].fd, map_request, checker);

                        request = data(map_request, fds[i].fd);
                        // std::cout <<request;
                        cherk = donner_flags(checker, fds[i].fd);

                        //         std::cout<<cherk<<std::endl;
                        //         std::cout<<request<<std::endl;
                        //   exit(0);
                        // 13223547%

                        // na9s

                        // std::cout <<cherk;
                        // exit(0);
                        // exit(0);
                        // body --- Gett  //
                        if (request.size() >= cherk)
                        {
                            // std::cout << "done" << std::endl;
                            // std::cerr << request << std::endl;
                            // exit(1);
                            bzero(buf, 1024);
                            std::cout << "request  \n";
                            std::string port, name_serveur;
                            geve_port_name(request, name_serveur, port);

                            ///-------data_serveur  Sened ----

                            //  exit(0);
                            int serveur_id = getServerId(data_conf.m_servers, atoi(port.c_str()), name_serveur);
                            feedRequest(serveur_id, data_conf.m_servers, request);
                            respense = sendResponse(serveur_id, data_conf.m_servers);
                           
                            //   std::cout<<respense;
                            //   exit(0);
                            // send(fds[i].fd,   respense.c_str(),    respense.length(), 0);
                            // exit(0);
                            // std::cout<< respense;
                            //             std::cerr<<respense;
                            // exit(0);
                            size_t p;
                            int j = 1;
                            size_t si = 0;
                            std::string re = respense.substr(0, 1000);
                            ;
                            re.clear();
                            size_t lenght;

                            lenght = re.size();
                            while (respense.size() >= 0 && si != respense.size())
                            {
                                std::string requ;
                                //  174080
                                requ = respense.substr(si, 1000);
                                // if(requ.size() >= 1000)
                                // {
                                //     std::cout<<requ.size()<<"(8888)\n";
                                //     break;
                                // }
                                // if(si == 59392 )
                                // {
                                //     std::cout<<requ<<"\n" ;

                                // }
                                // if(requ.size() >= 1024)
                                //     break;

                                si += requ.size();
                                //         std::cout<<"--------------------------\n";
                                //          std::cout<< si<<"---{}{{}{}}} \n";
                                //         std::cout<<lenght<<" ----- lennght \n";
                                //       std::cout<<respense.size()<<"\n";
                                //    std::cout<<"-------------------------------------\n"<<requ.size();
                                //     std::cout<<"-======\n";

                                p = send(fds[i].fd, requ.c_str(), requ.length(), 0);

                                // if(   p  <= 0 )
                                // {
                                //             std::cout<<"hellooo ---iiiiiiii\n";
                                // }
                                //    send(fds[i].fd,requ.c_str(), requ.length(), serveur_id);
                                // std::cout<<p<<" ---\n" ;
                                p = 1;
                                j++;
                                requ.clear();
                            }

                            // std::cout <<

                            request.clear();
 
                            map_request.erase(fds[i].fd);
                            checker.erase(fds[i].fd);

                            client.erase(std::find(client.begin(), client.end(), fds[i].fd));
                      
                                    
                            close(fds[i].fd);
                            fds.erase(fds.begin() + index_fds( fds  , fds[i].fd));
                            ////////////////////////////////////////////////

                            //  close(fds[i].fd);
                        }
                        // messgae wssel
                    }
                }
                // close(co);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
