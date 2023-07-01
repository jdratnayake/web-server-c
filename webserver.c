#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main()
{
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("webserver (socket) ");
        return 1;
    }
    printf("socket created successfully\n");

    // create the address to bind the socket to
    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&host_addr, host_addrlen) != 0)
    {
        perror("webserver (bind) ");
        return 1;
    }
    printf("socket successfully bound to address\n");

    // listen for incoming connections
    if (listen(sockfd, SOMAXCONN) != 0)
    {
        perror("webserver (listen)");
        return 1;
    }
    printf("server listening for connections\n");

    for(;;)
    {
        // accept incoming connections
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
    
        if (newsockfd < 0)
        {
            perror("webserver (accept)");
            continue;
        }

        close(newsockfd);
    }

    return 0;
} 