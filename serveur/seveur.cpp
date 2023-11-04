// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   main.cpp                                           :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: iedderqi <iedderqi@student.42.fr>          +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2023/10/26 14:23:56 by iedderqi          #+#    #+#             */
// /*   Updated: 2023/10/28 16:05:39 by iedderqi         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */


#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read



//one serveur 
int main(int ac , char **av)
{
    struct  sockaddr_in address ;
    socklen_t addrlen = sizeof(address);
    int  fd; 
    // address.sin_len = ;
    // address.sin_zero = ;
    if(  (fd = socket(  AF_INET, SOCK_STREAM ,0)) < 0 )
    {
           perror("soket faild  ");
        return(0);
    }
    //TODO 
    // inet_addr("192.168.1.100"); 
    address.sin_addr.s_addr =  INADDR_ANY;
    address.sin_family =  AF_INET;
    address.sin_port = htons(8080);
    std::cout<<"soket correcte\n";
    std::cout<<fd<<"\n";
    if(bind( fd,(struct sockaddr *) &address,sizeof(address) ) < 0 )
    {
        perror("bind failed");
        return(0);    
    }
    std::cout<<fd<<"\n";
    std::cout<<"bind  correcte\n";
  
    if(listen(fd, 3) < 0 )
    {
        perror("listen failed : ");
        return(0);
    }
    std::cout<<"listen Gooood !!!\n";
    std::cout<<fd<<"\n";
    int co = accept(fd,(struct sockaddr *) &address,(socklen_t*)&addrlen  ); 
    if(co < 0 )
    {
  
         perror("accept");
    }
    std::cout<<"accept Goood \n";
       std::cout<<co<<"\n";
 char buffer[100];
 ///buffer 
  size_t bytesRead = read(co, buffer, 100);
  std::cout<<buffer;
  std::string response = "Good talking to you\n";
  send(co, response.c_str(), response.size(), 0);
//  close(fd);
//  close(co);
 
    return(0);

}


 