#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main()
{
     struct sockaddr_in serv_addr;
    int fd = socket(AF_INET , SOCK_STREAM, 0 );
    if(fd < 0)
        perror("soket_errors : ");
    // std::cout<<"soket_good \n";

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    int status ;

     if ((status
         = connect(fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

     char* hello = "Hello from client";

     send(fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");
        char buffer[1024] = { 0 };
   size_t  valread = read(fd, buffer, 1024 - 1); // subtract 1 for the null terminator at the end
    printf("%s", buffer);
close(fd);


}