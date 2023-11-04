#include <vector>
#include <iostream>

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>      // For exit() and EXIT_FAILURE
#include <iostream>     // For cout
#include <unistd.h>     // For read
#include <vector>
#include <poll.h>

//one serveur whitmulti clients ::
 
int main()
{

     

    struct sockaddr_in adrese;
    socklen_t addrlen = sizeof(adrese);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("socket:felaid ");
        exit(0);
    }
    adrese.sin_addr.s_addr = INADDR_ANY;
    adrese.sin_family = AF_INET;
    adrese.sin_port = htons(8888);
    if (bind(fd, (struct sockaddr *)&adrese, sizeof(adrese)) < 0)
    {
        perror("bind Failed :");
        exit(0);
    }
    int ls = listen(fd, 3);
    if (ls)
    {

        perror("listen Failed : ");
        exit(0);
    }

    std::vector<struct pollfd> fds(1);
        fds[0].fd = fd;
        fds[0].events = POLLIN;

    while (true)
    {
         
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret == -1)
        {
            perror("POOLLL failed :");
            return (0);
        }
 
        
        for (int i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
               
                if (fds[i].fd == fd){
              
                    std::cout << "hello new\n" << std::endl;
                    int co = accept(fds[i].fd, NULL , NULL );
                    if(co < 0)
                    {
                        perror("Faild ::  acceot");
                        continue;
                    }
                    struct pollfd fl;
                    fl.fd = co ;
                    fl.events = POLLIN;
                    fds.push_back(fl);

                }
                else {
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
                    std::cout<<"bybye" << std::endl;
                }
                else
                {
                    std::cout<<buf;
                }
                // messgae wssel
                } 
                }
                    // close(fds[i].fd );
            }
        }
     

    return (0);
} 