

// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   main.cpp                                           :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: iedderqi <iedderqi@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2023/10/26 14:23:56 by iedderqi          #+#    #+#             */
/*   Updated: 2023/12/05 14:45:17 by iedderqi         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */

 
#include"servers.hpp"
 
int main(int ac, const char **av)
{
    (void)ac;
    try
    {
        /////////////////////////////
        ConfigParser data_conf(av);       //
        data_conf.readConfigFile();       //
        data_conf.checkBrackets();        ///
        checkServer(data_conf.m_servers); ///
        data_conf.closeDir();
        /////////////////////////////
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
        std::map<int, std::string> res;
        std::string request;
        std::vector<int> client;
        std::map<int, int> chunked;
        std::map<int, size_t> len_requeste;
        std::map<int, bool> connection;
        std::vector<struct pollfd> fds;
        //////////////////////////////
        create_soket(data_conf, serveur, fds);
        while (true)
        {
            int ret = poll(fds.data(), fds.size(), 0);
            if (ret == -1)
                throw std::runtime_error("poll error");
            for (size_t i = 0; i < fds.size(); i++)
            {
                if (fds[i].revents == POLLIN)
                {
                    if (!read_to_client(serveur, i, fds, client, map_request, checker, chunked, connection, data_conf, res))
                        break;
                   
                }
                if (fds[i].revents == POLLOUT)
                {
                    int cheker = 0;
                    follow_responsive(cheker, len_requeste, fds[i].fd, res);
                    if (!cheker)
                    {
                        delete_maps(map_request, res, len_requeste, chunked, checker, fds[i].fd);
                        if (connection.find(fds[i].fd) != connection.end() && connection[fds[i].fd] == 0)
                        {
                            connection.erase(fds[i].fd);
                            close_fd(fds[i].fd, client, fds);
                        }
                        else
                        {
                            connection.erase(fds[i].fd);
                            fds[i].events = POLLIN;
                            break;
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

// siege -b 127.0.0.1:8002
