/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tols_Second.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iedderqi <iedderqi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/11 20:24:25 by iedderqi          #+#    #+#             */
/*   Updated: 2023/12/16 14:59:52 by iedderqi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "servers.hpp"
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
        {

            port_name_serveur(inforamation(request, i + 5), port, name_serveur);
            return;
        }
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
            return;
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

    return (0);
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