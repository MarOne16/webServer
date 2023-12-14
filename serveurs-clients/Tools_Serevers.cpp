/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tools_Serevers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iedderqi <iedderqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 20:12:38 by iedderqi          #+#    #+#             */
/*   Updated: 2023/12/11 21:20:21 by iedderqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

 #include"servers.hpp"

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
        // number[i] = '\0';

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
        //         std::cout<<lengt<<" chek \n";

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

void request_inserer(char *buffer, int buff_size, int fd, std::map<int, std::string> &map_request, std::map<int, size_t> &checker, int &stop, std::map<int, int> &chunked)
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
        std::string request = std::string(buffer, buff_size);
        it->second += request;

        // std::cout << it->second << " -- \n";
        size_t founds = it->second.find("\r\n\r\n");
        if (j == 1 || founds == std::string::npos)
        {
            it_checker->second = content_lenght(it->second) + length_heder(it->second.substr(0, content_lenght(it->second)));
            if (j && it->second == "\r\n")
                stop = 1;
        }
        if (j && founds != std::string::npos)
        {

            if (is_chunked(it->second.substr(0, content_lenght(it->second))))
                chunked.insert(std::make_pair(fd, 1));
            else
                chunked.insert(std::make_pair(fd, 0));
        }

        if (founds != std::string::npos)
        {

            if (chunked[fd] == 0)
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

        request.clear();
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

void close_fd(int fd, std::vector<int> &client, std::vector<struct pollfd> &fds)
{
    close(fd);
    client.erase(std::find(client.begin(), client.end(), fd));
    fds.erase(fds.begin() + index_fds(fds, fd));
}



void accepte_client(int fd, std::vector<int> &client, std::vector<struct pollfd> &fds)
{
    int fd_client = accept(fd, NULL, NULL);
    fcntl(fd_client, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    if (fd_client > 0)
    {
        struct pollfd fl;
        fl.fd = fd_client;
        fl.events = POLLIN;
        client.push_back(fd_client);
        fds.push_back(fl);
    }
}
void follow_responsive(int &cheker, std::map<int, size_t> &lenght, int fd, std::map<int, std::string> request)

{

    std::string response;
    size_t lenght_response;
    std::string respons_find = request[fd];
    if (lenght.find(fd) == lenght.end())
    {
        cheker = 1;
        response = respons_find.substr(0, 1000);
        lenght_response = response.size() + 0;
        lenght.insert(std::make_pair(fd, lenght_response));
    }
    else
    {
        response = respons_find.substr(lenght[fd], 1000);
        lenght_response = response.size() + lenght[fd];
        lenght.erase(fd);
        lenght.insert(std::make_pair(fd, lenght_response));
        cheker = 1;
    }

    if (cheker)
    {
        if (send(fd, response.c_str(), response.length(), 0) == -1)
            cheker = 0;
        if (lenght_response == respons_find.size())
            cheker = 0;
    }
}

void partient_request(int &stop, int fd, ConfigParser data_conf, std::map<int, std::string> &response_map,
                      std::map<int, bool> &connection, std::vector<struct pollfd> &fds, int i, std::map<int, std::string> map_request)
{

    std::string respense;
    std::string request = map_request[fd];
    stop = 0;
    std::string port, name_host, name_serveur;
    geve_port_host(request, name_host, port);
    geve_port_serveur(request, name_serveur);
    int serveur_id = getServerId(data_conf.m_servers, atoi(port.c_str()), name_serveur, name_host);
    feedRequest(serveur_id, data_conf.m_servers, request);
    respense = sendResponse(serveur_id, data_conf.m_servers);
    connection.insert(std::make_pair(fd, data_conf.m_servers.find(serveur_id)->second.connection));
    response_map.insert(std::make_pair(fd, respense));
    fds[i].events = POLLOUT;
}

int read_to_client(std::vector<int> server, int i, std::vector<struct pollfd> &fds, std::vector<int> &client,
                   std::map<int, std::string> &map_request, std::map<int, size_t> &checker, std::map<int, int> &chunked, std::map<int, bool> &connection, ConfigParser data_conf, std::map<int, std::string> &res)
{
    int stop = 0;
    if (std::find(server.begin(), server.end(), fds[i].fd) != server.end())
    {
        accepte_client(fds[i].fd, client, fds);
        return (0);
    }
    else if (std::find(client.begin(), client.end(), fds[i].fd) != client.end())
    {
        char *buf = new char[ 1024 ];
        if (!buf)
            throw std::runtime_error("malloc : : failed  \n");
        int rec = recv(fds[i].fd, buf, 1023, 0);
        if (rec <= 0)
        {
            
            delete [] buf;
            stop = 0;
            map_request.erase(fds[i].fd);
            checker.erase(fds[i].fd);
            chunked.erase(fds[i].fd);
            close_fd(fds[i].fd, client, fds);
        }
        else
        {
            request_inserer(buf, rec, fds[i].fd, map_request, checker, stop, chunked);
            if (stop == 1)
            {
                 delete [] buf;
                partient_request(stop, fds[i].fd, data_conf, res, connection, fds, i, map_request);
            }
            else
                delete [] buf;
        }
    }

    return (1);
}

void add_serveur(std::vector<struct pollfd> &fds, std::vector<int> serveur)
{
    for (size_t i = 0; i < serveur.size(); i++)
    {
        struct pollfd poll;
        poll.fd = serveur[i];
        poll.events = POLLIN;
        fds.push_back(poll);
    }
}

void create_soket(ConfigParser data_conf, std::vector<int> &serveur, std::vector<struct pollfd> &fds)
{
    std::vector<int> port;
    ports(port, data_conf.m_servers);
    for (unsigned int i = 0; i < data_conf.getNumber_ofServers(); i++)
    {
        struct sockaddr_in adrese;
        // int sockfd = socket(domain, type, protocol)
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
            throw std::runtime_error("socket : : failed  \n");
        int opt = 1;
        ///  sgv send
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
            throw std::runtime_error("setsockopt : : failed  \n");
        if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(int)) == -1)
            throw std::runtime_error("setsockopt : : failed  \n");
        fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        /// non-blocking file descriptors
        //   host

        //  server_address.sin_family = AF_INET;

        // Convert the loopback address "127.0.0.1" to binary format and store it in sin_addr.s_addr
        adrese.sin_family = AF_INET;
        std::map<unsigned int, server>::iterator it = data_conf.m_servers.find(i);

        if (inet_pton(AF_INET, it->second.host.c_str(), &adrese.sin_addr) <= 0)
            throw std::runtime_error("inet_pton : : failed  \n");
        // adrese.sin_addr.s_addr = INADDR_ANY;
        adrese.sin_port = htons(port[i]);
        if (bind(fd, (struct sockaddr *)&adrese, sizeof(adrese)) < 0)
            throw std::runtime_error("bind : : failed  \n");
        if (listen(fd, 1024) < 0)
            throw std::runtime_error("listen : : failed  \n");
        serveur.push_back(fd);
    }
    add_serveur(fds, serveur);
}