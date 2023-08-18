#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CONN 3
#define RESPONSE "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello, world!"

int main()
{
    // store the file descriptors (identifiers) for the server socket and the new client socket
    // A file descriptor is a unique identifier for an open socket
    int server_fd, new_socket;
    // used to represent an IPv4 socket address
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // AF_INET means IPv4 address family
    // SOCK_STREAM = indicate TCP socket
    // 0 = OS select an appropriate protocol based on the provided features
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully\n");

    address.sin_family = AF_INET;
    // sin_addr = represents the IP address to that the socket will be bound
    // INADDR_ANY is a constant that represents all available network interfaces 
    // on the host. This means the server will listen on all available network interfaces
    address.sin_addr.s_addr = INADDR_ANY;
    // converts the port number from host byte order to network byte order
    address.sin_port = htons(PORT);

    // associate the server socket with a specific IP address and port number
    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    printf("IP address bind successfully\n");

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening for connections\n");

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Accept the connection\n");

        // 0 = indicate sending
        send(new_socket, RESPONSE, strlen(RESPONSE), 0);
        printf("Response sent to client\n");
        close(new_socket);
    }

    return 0;
}