

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

#include "../Response/webserver.hpp"
#include <fstream>
#include <arpa/inet.h>

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

void geve_port_host(std::string request, std::string &name_serveur, std::string &port)
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

void geve_port_serveur(std::string request, std::string &name_serveur)
{

    for (size_t i = 0; i < request.size(); i++)
    {
        if (request[i] == '\r')
            i++;
        if (request[i] == '\n')
            i++;
        if (i + 4 < request.size() && is_Host(request.substr(i, 4)))
        {
            i = i + 4;
            i += 2;
            while (request[i] != '\r')
            {
                name_serveur += request[i];
                i++;
            }
            //  ignore_espace(name_serveur);
        }
        // port_name_serveur(inforamation(request, i + 5), port, name_serveur);
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
        start_pos += to.length();
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
            return (calcule.size());
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
                // std::cout<<
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

std::string traitement_chunkde(size_t &i, int &state, std::string request, std::map<int, int> &satate, int fd, std::map<int, std::string> &numbers)
{

    std::string chunked;
    size_t lenght;

    while (i == 0 || state == 1)
    {
        /// that mind complier
        std::string cheked;
        std::string number;

        while (is_digit(request[i]))
        {
            number += request[i];
            i++;
        }
        std::stringstream stream(number);
        stream >> lenght;
        if (!request[i])
        {
            //\n
            satate[fd] = -2;
            if (numbers.find(fd) != numbers.end())
                numbers[fd] = number;
            else
                numbers.insert(std::make_pair(fd, number));
            return (chunked);
        }
        if (!request[i + 1])
        {
            //\r
            satate[fd] = -3;
            if (numbers.find(fd) != numbers.end())
                numbers[fd] = number;
            else
                numbers.insert(std::make_pair(fd, number));
            return (chunked);
        }
        i += 2;
        if (lenght == 0)
        {
            satate[fd] = 0;
            chunked += "\r\n";
            return (chunked);
        }
        for (size_t j = i; j < request.size(); j++ && i++)
        {
            if (is_digit(request[j]) && cheked.size() >= (lenght + 2))
            {
                state = 1;
                break;
            }
            cheked += request[j];
            chunked += request[j];
        }
        if (cheked.size() == (lenght + 2))
            satate[fd] = -1;

        else
            satate[fd] = (lenght - cheked.size());
    }
    return (chunked);
}
std::string check_complier_chunkerd(std::string request, std::map<int, int> &satate, int fd, std::map<int, std::string> &numbers)
{
    std::string chunked;

    size_t lenght = 0;
    int state = 0;
    size_t i = 0;

    // 1 // that is mind complier
    if (satate.empty() || contente_second(satate, fd) != -1)
        chunked = traitement_chunkde(i, state, request, satate, fd, numbers);

    // that is mind not complier
    else if (contente_second(satate, fd) > 0)
    {
        int fix = contente_second(satate, fd);
        for (i = 0; (int)i < fix; i++)
            chunked += request[i];
        i += 2;
        chunked += traitement_chunkde(i, state, request, satate, fd, numbers);
    }
    else if (contente_second(satate, fd) == -2)
    {
        // r
        if (is_digit(request[i]))
        {
            std::string num;
            num = numbers[fd];
            while (is_digit(request[i]))
            {
                num += request[i];
                i++;
            }
            std::stringstream stream(num);
            stream >> lenght;
            for (size_t j = i; j < (lenght + 2); j++)
            {
                chunked += request[j];
                i++;
            }
        }
        else
            i += 2;
        chunked += traitement_chunkde(i, state, request, satate, fd, numbers);
    }
    else if (contente_second(satate, fd) == -3)
    {
        // n

        i++;
        chunked = traitement_chunkde(i, state, request, satate, fd, numbers);
    }
    // std::cout<<request;
    // exit(0);

    return (chunked);
}

void converture_hex(std::string hex, size_t &lenght)
{
    // std::cout << "faild :" << hex << "\n";
    lenght = std::stoul(hex, 0, 16);
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

        size_t lengt;
        for (i = j; i < request.size() && request[i] != '\r'; i++)
            number += request[i];
        // number[i] = '\0';
        converture_hex(number, lengt);
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
            size_t lenght;
            std::map<int, std::string>::iterator itter = num.find(fd);
            std::string number;
            number = itter->second;
            std::string size;
            for (j = 0; j < request.size() && request[j] != '\r'; j++)
                size += request[j];
            number += size;
            converture_hex(number, lenght);
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
        if (j == 1)
        {
            it_checker->second = content_lenght(request) + length_heder(request.substr(0, content_lenght(request)));
            if (is_chunked(request.substr(0, content_lenght(request))))
                chunked.insert(std::make_pair(fd, 1));
            else
                chunked.insert(std::make_pair(fd, 0));
        }

        if (chunked[fd] == 0)
        {
            it->second += request;
            if (it->second.size() >= it_checker->second)
                stop = 1;
        }
        else
        {
            it->second += request;
            size_t founds = it->second.find("\r\n0\r\n\r\n");
            if (founds != std::string::npos)
                stop = 1;
        }

        request.clear();
    }
    return;
}
// flag :  complier  ||  finale : request ||  flage : note complier  (number compler )
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
        ports(port, data_conf.m_servers);
        for (unsigned int i = 0; i < data_conf.getNumber_ofServers(); i++)
        {
            struct sockaddr_in adrese;
            socklen_t addrlen = sizeof(adrese);
            // int sockfd = socket(domain, type, protocol)
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd < 0)
                return (0);
            int opt = 1;
            ///  sgv send
            if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
                return (0);
            if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(int)) == -1)
                return (0);
            fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
            /// non-blocking file descriptors
            //   host

            //  server_address.sin_family = AF_INET;

            // Convert the loopback address "127.0.0.1" to binary format and store it in sin_addr.s_addr
            adrese.sin_family = AF_INET;
            std::map<unsigned int, server>::iterator it = data_conf.m_servers.find(i);

            if (inet_pton(AF_INET, it->second.host.c_str(), &adrese.sin_addr) <= 0)
            {
               printf("inet_pton  : : failed  \n");
                return (0);
            }

            // adrese.sin_addr.s_addr = INADDR_ANY;
            adrese.sin_port = htons(port[i]);
            if (bind(fd, (struct sockaddr *)&adrese, sizeof(adrese)) < 0)
            {
                printf("bind : : Can't assign requested address\n");
                return (0);
            }
            if (listen(fd, 1024) < 0)
            {
                printf("listen : : Can't lisen \n");
                return (0);
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
        std::map<int, int> chunked;
        std::map<int, size_t> len_requeste;
        std::map<int, bool> connection;
        // size_t cherk;
        int stop = 0;
        while (true)
        {
            int ret = poll(fds.data(), fds.size(), 0);
            if (ret == -1)
                return (0);
            for (size_t i = 0; i < fds.size(); i++)
            {
                if (fds[i].revents & POLLIN)
                {
                    if (std::find(file.begin(), file.end(), fds[i].fd) != file.end())
                    {

                        int co = accept(fds[i].fd, NULL, NULL);
                        fcntl(co, F_SETFL, O_NONBLOCK, FD_CLOEXEC);

                        if (co > 0)
                        {
                            struct pollfd fl;
                            fl.fd = co;
                            fl.events = POLLIN;
                            client.push_back(co);
                            fds.push_back(fl);
                        }
                        break;
                    }
                    else if (std::find(client.begin(), client.end(), fds[i].fd) != client.end())
                    {
                        char *buf = (char *)malloc(1024);
                        //
                        if (!buf)
                            return (0);
                        int rec = recv(fds[i].fd, buf, 1023, 0);
                        if (rec == 0)
                        {
                            free(buf);
                            stop = 0;
                            map_request.erase(fds[i].fd);
                            checker.erase(fds[i].fd);
                            chunked.erase(fds[i].fd);
                            client.erase(std::find(client.begin(), client.end(), fds[i].fd));
                            close(fds[i].fd);
                            fds.erase(fds.begin() + index_fds(fds, fds[i].fd));
                        }
                        else if (rec == -1)
                            continue;
                        else
                        {
                            request_inserer(buf, rec, fds[i].fd, map_request, checker, stop, chunked);
                            free(buf);
                            if (stop == 1)
                            {
                                request = data(map_request, fds[i].fd);
                                stop = 0;
                                std::string port, name_host, name_serveur;
                                // geve_port_host(request, name_host, port);
                                // geve_port_serveur(request, name_serveur);
                                // std::cout << request;
                                // std::cout << name_serveur << "\n";
                                // std::cout << name_host;
                                // // TODO mqaos implimentation
                                // exit(0);
                                int serveur_id = getServerId(data_conf.m_servers, atoi(port.c_str()), name_host);
                                feedRequest(serveur_id, data_conf.m_servers, request);
                                respense = sendResponse(serveur_id, data_conf.m_servers);
                                connection.insert(std::make_pair(fds[i].fd, data_conf.m_servers.find(serveur_id)->second.connection));
                                res.insert(std::make_pair(fds[i].fd, respense));
                                fds[i].events = POLLOUT;
                                break;
                            }
                        }
                    }
                }

                if (fds[i].revents & POLLOUT)
                {

                    int cheker = 0;
                    size_t t;
                    std::string respons_find;
                    respons_find = res[fds[i].fd];
                    std::string requ;
                    if (len_requeste.find(fds[i].fd) == len_requeste.end())
                    {

                        cheker = 1;
                        requ = respons_find.substr(0, 1000);
                        t = requ.size() + 0;
                        len_requeste.insert(std::make_pair(fds[i].fd, t));
                    }
                    else
                    {
                        requ = respons_find.substr(len_requeste[fds[i].fd], 1000);
                        t = requ.size() + len_requeste[fds[i].fd];
                        len_requeste.erase(fds[i].fd);
                        len_requeste.insert(std::make_pair(fds[i].fd, t));
                        cheker = 1;
                    }

                    if (cheker)
                    {
                        if (send(fds[i].fd, requ.c_str(), requ.length(), 0) == -1)
                            cheker = 0;
                        if (t == respons_find.size())
                            cheker = 0;
                    }
                    requ.clear();
                    if (!cheker)
                    {
                        respons_find.clear();
                        len_requeste.erase(fds[i].fd);
                        map_request.erase(fds[i].fd);
                        checker.erase(fds[i].fd);
                        chunked.erase(fds[i].fd);
                        res.erase(fds[i].fd);
                        if (connection.find(fds[i].fd) != connection.end() && connection[fds[i].fd] == 0)
                        {
                            connection.erase(fds[i].fd);
                            close(fds[i].fd);
                            client.erase(std::find(client.begin(), client.end(), fds[i].fd));
                            fds.erase(fds.begin() + index_fds(fds, fds[i].fd));
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

// siege -b --delay=0.5 --file=url.txt --concurrent=15 --no-parser
//  siege --delay=0.5 --file=url.txt --internet --verbose --reps=200 --concurrent=15 --no-parser
// siege -b 127.0.0.1:8002

/// omz_termsupport_cwd:3: pipe failed: too many open files in system
// zsh: pipe failed: too many open files in system