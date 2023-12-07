#include "../Response/webserver.hpp"
#include <fstream>

///----do
/// multi serveurs whit multi clients
// TODO
/// service_name
/// chankese request
/// merege requset whit serveur

///---shold do
/// chuundek
/// clear code
/// port if need
// CGI

//""request chunk

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
    std::cout << "traitement_chunk" << std::endl;
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

void request_inserer(char buffer[1024], int buff_size, int fd, std::map<int, std::string> &map_request)
{

    if (map_request.find(fd) == map_request.end())
        map_request.insert(std::make_pair(fd, ""));

    std::map<int, std::string>::iterator it = map_request.find(fd);

    if (it != map_request.end())
    {
        std::string request = std::string(buffer, buff_size);
        it->second += request;
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

        std::map<int, std::string> number;
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
                int j = 0;
                if (fds[i].revents & POLLIN)
                {
                    if (std::find(file.begin(), file.end(), fds[i].fd) != file.end())
                    {
                        int co = accept(fds[i].fd, NULL, NULL);
                        fcntl(co, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
                        if (co < 0)
                            perror("Faild ::  accept");
                        else
                        {
                            struct pollfd fl;
                            fl.fd = co;
                            fl.events = POLLIN;
                            client.push_back(co);
                            fds.push_back(fl);
                            break;
                        }
                    }
                    else if (std::find(client.begin(), client.end(), fds[i].fd) != client.end())
                    {

                        bzero(buf, 1024);
                        int rec = recv(fds[i].fd, buf, 1024, 0);
                        if (rec < 0)
                            perror("recv");
                        else if (rec < 1024)
                        {
                            if (rec > 0)
                            {
                                buf[rec] = '\0';
                                request += buf;
                            }
                            std::cout<<"--------- \n";
                            std::cerr <<request;
                            std::cout<<"--------\n";
                            std::string port, name_serveur;
                            geve_port_name(request, name_serveur, port);
                            int serveur_id = getServerId(data_conf.m_servers, atoi(port.c_str()), name_serveur);
                            feedRequest(serveur_id, data_conf.m_servers, request);
                            respense = sendResponse(serveur_id, data_conf.m_servers);
                            size_t si = 0;
                            while (respense.size() >= 0 && si != respense.size())
                            {
                                std::string requ;
                                requ = respense.substr(si, 1000);
                                si += requ.size();
                                if (send(fds[i].fd, requ.c_str(), requ.length(), 0) <= 0)
                                {
                                    std::cout << "byby \n";
                                    request.clear();
                                    map_request.erase(fds[i].fd);
                                    client.erase(std::find(client.begin(), client.end(), fds[i].fd));
                                    close(fds[i].fd);
                                    fds.erase(fds.begin() + index_fds(fds, fds[i].fd));
                                    j = 1;
                                    break;
                                }
                                requ.clear();
                            }
                            if (j == 0)
                            {
                                request.clear();
                                map_request.erase(fds[i].fd);
                                client.erase(std::find(client.begin(), client.end(), fds[i].fd));
                                close(fds[i].fd);
                                fds.erase(fds.begin() + index_fds(fds, fds[i].fd));
                            }
                        }

                        else
                        {
                            request_inserer(buf, rec, fds[i].fd, map_request);
                            request = data(map_request, fds[i].fd);
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