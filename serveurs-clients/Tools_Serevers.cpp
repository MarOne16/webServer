/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tools_Serevers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iedderqi <iedderqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 20:12:38 by iedderqi          #+#    #+#             */
/*   Updated: 2023/12/17 11:01:07 by iedderqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "servers.hpp"

int index_fds(std::vector<struct pollfd> fds, int fd)
{
    int i = 0;
    std::vector<struct pollfd>::iterator it;

    for (it = fds.begin(); it != fds.end(); it++)
    {

        if (it->fd == fd)
            return (i);

        i++;
    }
    return (-1);
}

int is_chunked(std::string request)
{

    for (size_t i = 0; i < request.size(); i++)
    {
        while (request[i] == '\r' || request[i] == '\n' || request[i] == ' ')
            i++;
        if (i + 17 < request.size())
        {
            std::string chceck = request.substr(i, 17);

            if (chceck == "Transfer-Encoding")
            {

                i += 17;
                std::string number;
                i += 2;
                while (request[i] == ' ')
                    i++;
                number = request.substr(i, 7);
                if (number == "chunked")
                    return (1);
            }
        }
    }
    return (0);
}

int contente_second(std::map<int, int> &state, int fd)
{
    std::map<int, int>::iterator it;
    std::map<int, int>::iterator itt = state.begin();
    for (it = state.begin(); it != state.end(); it++)
    {
        if (it->first == fd)
        {
            state.begin() = itt;
            return (it->second);
        }
    }
    state.begin() = itt;
    return (-1);
}

int digital(std::string number)
{
    for (size_t i = 0; i < number.size(); i++)
        if (!is_digit(number[i]))
            return (0);
    return (1);
}
std::string chunked_ramase(std::string request, std::map<int, size_t> &flags, int &stop, size_t &i, int fd, std::map<int, char> &cha,
                           std::map<int, std::string> &num)
{
    int state = 1;
    std::string chunked;
    size_t j = i;

    while (state)
    {
        std::string number;

        size_t lengt = 0;
        for (i = j; i < request.size() && request[i] != '\r'; i++)
            number += request[i];
        if (!request[i] && request[i - 1] != '\r')
        {

            lengt += 2;
            flags.insert(std::make_pair(fd, lengt));
            num.insert(std::make_pair(fd, number));
            return (chunked);
        }
        if (!request[i] && request[i - 1] == '\r')
        {

            lengt += 2;
            flags.insert(std::make_pair(fd, lengt));
            cha.insert(std::make_pair(fd, '\n'));
            return (chunked);
        }

        i += 2;
        // cas 0
        lengt += 2;
        if (lengt == 2)
        {
            chunked += "\r\n";
            stop = 1;
            return (chunked);
        }

        std::string last;
        for (j = i; j < request.size() && last.size() != lengt; j++)
        {
            chunked += request[j];
            last += request[j];
        }

        if (!request[j])
        {

            size_t t = (lengt)-last.size();

            if (t > 0)
                flags.insert(std::make_pair(fd, t));
            return (chunked);
        }
        i = j;

        last.clear();
    }
    return ("NULL");
}
std::string chunked_request(std::string request, std::map<int, size_t> &flags, int &stop, int fd, std::map<int, char> &cha, std::map<int, std::string> &num)
{
    std::string chunked;
    size_t i = 0;
    size_t j = 0;
    if (num.find(fd) != num.end())
    {
        if (request[j] == '\r')
        {

            if ((flags[fd] - 2) == 0)
            {
                stop = 1;
                return ("\r\n");
            }
            j += 2;
        }
        else
        { /// r/n1
            // 012

            flags.erase(fd);
            size_t lenght = 0;
            std::map<int, std::string>::iterator itter = num.find(fd);
            std::string number;
            number = itter->second;
            std::string size;
            for (j = 0; j < request.size() && request[j] != '\r'; j++)
                size += request[j];
            number += size;

            if (lenght == 0)
            {
                stop = 1;
                return ("\r\n");
            }
            lenght += 2;
            flags.insert(std::make_pair(fd, lenght));
            j += 2;
        }
        num.erase(fd);
    }

    std::map<int, size_t>::iterator it = flags.find(fd);
    std::map<int, char>::iterator itt = cha.find(fd);
    if (itt != cha.end())
    {
        if ((flags[fd] - 2) == 0)
        {
            stop = 1;
            return ("\r\n");
        }
        if (itt->second == '\n')
            j++;
        cha.erase(fd);
    }
    if (it != flags.end() && it->second > 0)
    {
        size_t size;
        for (i = j; i < request.size() && chunked.size() != it->second; i++)
            chunked += request[i];
        size = it->second - chunked.size();
        if (!request[i])
        {
            flags.erase(fd);
            if (size > 0)
                flags.insert(std::make_pair(fd, size));
            return (chunked);
        }
        flags.erase(fd);
    }
    chunked += chunked_ramase(request, flags, stop, i, fd, cha, num);
    return (chunked);
}

void request_inserer(char *buffer, int buff_size, int fd, Servers &serveur, int &stop)
{
    int j = 0;
    if (serveur.map_request.find(fd) == serveur.map_request.end())
    {
        serveur.map_request[fd] = "";
        serveur.checker[fd] = 0;
        j = 1;
    }
    std::map<int, std::string>::iterator it = serveur.map_request.find(fd);
    std::map<int, size_t>::iterator it_checker = serveur.checker.find(fd);
    if (it != serveur.map_request.end() && it_checker != serveur.checker.end())
    {
        it->second += std::string(buffer, buff_size);
        size_t founds = it->second.find("\r\n\r\n");
        if (j == 1 || founds == std::string::npos)
        {
            it_checker->second = content_lenght(it->second) + length_heder(it->second.substr(0, content_lenght(it->second)));

            if (j && it->second == "\r\n")
                stop = 1;
            serveur.len_read[fd] = length_heder(it->second.substr(0, content_lenght(it->second)));
        }
        if (j && founds != std::string::npos)
        {

            if (is_chunked(it->second.substr(0, content_lenght(it->second))))
                serveur.chunked[fd] = 1;
            else
                serveur.chunked[fd] = 0;
        }

        if (founds != std::string::npos)
        {

            if (serveur.chunked[fd] == 0)
            {
                if (it->second.size() >= it_checker->second)
                    stop = 1;
            }
            else
            {
                size_t founds = it->second.find("\r\n0\r\n\r\n");
                if (founds != std::string::npos)
                    stop = 1;
            }
        }
    }
    return;
}

void delete_maps(std::map<int, std::string> &request, std::map<int, std::string> &responce, std::map<int, size_t> &lent_response, std::map<int, int> &is_chunked, std::map<int, size_t> &cheker, int fd)
{
    request.erase(fd);
    responce.erase(fd);
    lent_response.erase(fd);
    is_chunked.erase(fd);
    cheker.erase(fd);
}

void close_fd(int fd, Servers &serveur)
{
    close(fd);
    serveur.client.erase(std::find(serveur.client.begin(), serveur.client.end(), fd));
    serveur.fds.erase(serveur.fds.begin() + index_fds(serveur.fds, fd));
}

void accepte_client(int fd, Servers &serveur)
{
    int fd_client = accept(fd, NULL, NULL);
    fcntl(fd_client, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    if (fd_client > 0)
    {
        struct pollfd fl;
        fl.fd = fd_client;
        fl.events = POLLIN;
        serveur.client.push_back(fd_client);
        serveur.fds.push_back(fl);
    }
}

void follow_responsive(int &cheker, int fd, Servers &serveur)
{
    size_t sen = 0;

    sen = send(fd, serveur.response_map[fd].c_str(), serveur.response_map[fd].length(), 0);
    if (sen <= 0 || serveur.response_map[fd].empty())
    {
        cheker = 0;
        return;
    }
    if (sen < serveur.response_map[fd].length())
    {
        serveur.response_map[fd] = serveur.response_map[fd].substr(sen);
        cheker = 1;
    }
    else
        cheker = 0;
}

void partient_request(int &stop, int fd, ConfigParser data_conf, int i, Servers &sereur)
{
    stop = 0;
    sereur.len_read.erase(sereur.fds[i].fd);
    std::string port, name_host, name_serveur;
    geve_port_host(sereur.map_request[fd], name_host, port);
    geve_port_serveur(sereur.map_request[fd], name_serveur);
    int serveur_id = getServerId(data_conf.m_servers, stosize_t(port), name_serveur, name_host);
    feedRequest(serveur_id, data_conf.m_servers, sereur.map_request[fd]);

    sereur.response_map[fd] = sendResponse(serveur_id, data_conf.m_servers);
    sereur.connection[fd] = data_conf.m_servers.find(serveur_id)->second.connection;

    sereur.fds[i].events = POLLOUT;
}

int read_to_client(Servers &sereur, int i, ConfigParser data_conf)
{

    int stop = 0;
    size_t read_len = 1048576;
    if (std::find(sereur.serveur.begin(), sereur.serveur.end(), sereur.fds[i].fd) != sereur.serveur.end())
        accepte_client(sereur.fds[i].fd, sereur);
    else if (std::find(sereur.client.begin(), sereur.client.end(), sereur.fds[i].fd) != sereur.client.end())
    {
        if (sereur.len_read.find(sereur.fds[i].fd) != sereur.len_read.end())
            read_len = sereur.len_read[sereur.fds[i].fd];
        char *buf = new char[read_len + 1];
        if (!buf)
            throw std::runtime_error("malloc : : failed  \n");

        int rec = recv(sereur.fds[i].fd, buf, read_len, 0);
        if (rec <= 0)
        {
            delete[] buf;
            stop = 0;
            sereur.len_read.erase(sereur.fds[i].fd);
            sereur.map_request.erase(sereur.fds[i].fd);
            sereur.checker.erase(sereur.fds[i].fd);
            sereur.chunked.erase(sereur.fds[i].fd);
            close_fd(sereur.fds[i].fd, sereur);
        }
        else
        {
            request_inserer(buf, rec, sereur.fds[i].fd, sereur, stop);
            if (stop == 1)
            {
                delete[] buf;
                partient_request(stop, sereur.fds[i].fd, data_conf, i, sereur);
            }
            else
                delete[] buf;
        }
    }

    return (1);
}

void add_serveur(Servers &serveur)
{
    for (size_t i = 0; i < serveur.serveur.size(); i++)
    {
        struct pollfd poll;
        poll.fd = serveur.serveur[i];
        poll.events = POLLIN;
        serveur.fds.push_back(poll);
    }
}

void create_soket(ConfigParser data_conf, Servers &serveur)
{
    std::vector<int> port;
    ports(port, data_conf.m_servers);
    for (unsigned int i = 0; i < data_conf.getNumber_ofServers(); i++)
    {
        struct sockaddr_in adrese;
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
            throw std::runtime_error("socket : : failed  \n");
        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
            throw std::runtime_error("setsockopt : : failed  \n");
        if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(int)) == -1)
            throw std::runtime_error("setsockopt : : failed  \n");
        fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        adrese.sin_family = AF_INET;
        std::map<unsigned int, server>::iterator it = data_conf.m_servers.find(i);
        if (inet_pton(AF_INET, it->second.host.c_str(), &adrese.sin_addr) <= 0)
            throw std::runtime_error("inet_pton : : failed  \n");
        adrese.sin_port = htons(port[i]);
        if (bind(fd, (struct sockaddr *)&adrese, sizeof(adrese)) < 0)
            throw std::runtime_error("bind : : failed  \n");
        if (listen(fd, SOMAXCONN) < 0)
            throw std::runtime_error("listen : : failed  \n");
        serveur.serveur.push_back(fd);
    }
    add_serveur(serveur);
}