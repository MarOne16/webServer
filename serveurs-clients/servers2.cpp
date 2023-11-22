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
void ports(std::vector<int> &port, std::map<unsigned int, server> &data_serv)
{
    std::map<unsigned int, server>::iterator itb = data_serv.begin();
    std::map<unsigned int, server>::iterator ite = data_serv.end();
    while (itb != ite)
    {
        port.push_back((itb->second.port));
        std::cout << (itb->second.port) << std::endl;
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

size_t lenght_heder(std::string request)
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

void request_inserer(std::string buffer, int fd, std::map<int, std::string> &map_request, std::map<int, size_t> &checker)
{

    if (map_request.find(fd) == map_request.end())
    {
     
        map_request.insert(std::pair< int, std::string>(fd, ""));
        checker.insert(std::pair< int,int>(fd, 1337));
        
    }
    std::map<int, std::string>::iterator it;
    std::map<int, size_t>::iterator it_checker = checker.begin();
    for (it = map_request.begin(); it != map_request.end(); it++)
    {

        if (it->first == fd && it_checker->first == fd)
        {
            it->second += buffer;

            it_checker->second = content_lenght(it->second) + lenght_heder(it->second.substr(0, content_lenght(it->second)));
            

            return;
        }
        it_checker++;
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
        int flag = 0;
        size_t cherk;
        while (true)
        {
            int ret = poll(fds.data(), fds.size(), -1);
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

                    if (std::find(file.begin(), file.end(), fds[i].fd) != file.end())
                    {

                        int co = accept(fds[i].fd, NULL, NULL);
                        fcntl(co, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
                        if (co < 0)
                        {
                            perror("Faild ::  acceot");
                            continue;
                        }
                        struct pollfd fl;
                        fl.fd = co;
                        fl.events = POLLIN;
                        fds.push_back(fl);
                    }
                    else
                    {
                        // client bye bye

                        char buf[1024];
                        bzero(buf, 1024);
                        int rec = recv(fds[i].fd, buf, 1024, 0);

                        if (rec < 0)
                        {

                            perror("recv");
                            exit(1);
                        }
                        if (rec == 0)
                        {
                            
                            fds.erase(fds.begin() + i);

                            std::cout << "bybye" << std::endl;
                        }
                        // request += std::string(buf, rec);
                        // if (flag == 0)
                        // cherk = content_lenght(request) + lenght_heder(request.substr(0, content_lenght(request)));
                        request_inserer(buf, fds[i].fd, map_request, checker);

                        if (flag == 0)
                            cherk = donner_flags(checker, fds[i].fd);
                        // 13223547%

                        flag = 1;
                        request = data(map_request, fds[i].fd); // std::cout<<request;
                        // std::cout<<cherk;
                        // exit(0);
                        // exit(0);

                        if (request.size() >= cherk)
                        {

                            puts("this is the server --- \n");
                            std::string port, name_serveur;
                            geve_port_name(request, name_serveur, port);

                            // std::ofstream outputFile("output.txt"); // create a new output file or overwrite an existing one

                            // if (outputFile.is_open())
                            // {                          // check if the file was opened successfully
                            //     outputFile << request; // write data to the file
                            //     outputFile.close();    // close the file when done
                            // }
                            // else
                            // {
                            //     std::cerr << "Error opening file\n";
                            // }

                            // // std::cout << request;
                            // exit(0);

                            // std::ostream &operator<<(std::ostream &os,
                            //                     const std::vector<int > &vector)
                            // {
                            //     // Printing all the elements
                            //     // using <<
                            //     for (auto element : vector)
                            //     {
                            //         os << element << " ";
                            //     }
                            //     return os;
                            // }
                            int serveur_id = getServerId(data_conf.m_servers, atoi(port.c_str()), name_serveur);
                            feedRequest(serveur_id, data_conf.m_servers, request);
                            // //TODO send response to client
                            std::cout << "OK" << std::endl;

                            respense = sendResponse(serveur_id, data_conf.m_servers);

                            // std::cout << "\n"
                            //           << respense << "    \n";
                                //// get != contenrt lenght 
                            ////////////////////////////////////////////////
                            send(fds[i].fd, respense.c_str(), respense.length(), serveur_id);
                  std::map<int ,std::string >::iterator it =map_request.begin();
                            while (it != map_request.end())
                            {
                                std::cout << "Key: " << it->first
                                     << ", Value: " << it->second << std::endl<<"((_----(----))))))\n";
                                ++it;
                            }
                             close(fds[i].fd); 
                             std::cout<<map_request.size();
                             std::cout<<"----------------- \n";
                            // std::cout<<fds[i].fd;

                            //   fds.erase(fds.begin() + i);
                            //

                            // clode fie if  request finale
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
