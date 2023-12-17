

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

#include "servers.hpp"

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
        Servers serveur;

        //////////////////////////////
        create_soket(data_conf, serveur);
        while (true)
        {
            size_t l = serveur.fds.size();

            int ret = poll(serveur.fds.data(), serveur.fds.size(), 0);
            if (ret == -1)
                throw std::runtime_error("poll error");
            for (size_t i = 0; i < l; i++)
            {
                if (serveur.fds[i].revents == POLLIN)
                    read_to_client(serveur, i, data_conf);
                else if (i < serveur.fds.size() && serveur.fds[i].revents == POLLOUT)
                {
                    int cheker = 0;
                    follow_responsive(cheker, serveur.fds[i].fd, serveur);
                    if (!cheker)
                    {
                        delete_maps(serveur.map_request, serveur.response_map, serveur.len_requeste, serveur.chunked, serveur.checker, serveur.fds[i].fd);
                        if (serveur.connection.find(serveur.fds[i].fd) != serveur.connection.end() && serveur.connection[serveur.fds[i].fd] == 0)
                        {

                            serveur.connection.erase(serveur.fds[i].fd);
                            close_fd(serveur.fds[i].fd, serveur);
                        }
                        else
                        {
                            serveur.connection.erase(serveur.fds[i].fd);
                            serveur.fds[i].events = POLLIN;
                            serveur.fds[i].revents = 0;
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
