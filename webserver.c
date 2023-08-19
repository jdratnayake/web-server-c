#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define PORT 8080
#define MAX_CONN 3
#define BUFFER_SIZE 1024

void send_regular(int client_socket, const char *filename)
{
    int filefd = open(filename, O_RDONLY);

    if (filefd == -1)
    {
        perror("File open error");
        exit(1);
    }

    struct stat filestat;
    fstat(filefd, &filestat);

    off_t offset = 0;
    ssize_t sent_bytes = 0;

    while (sent_bytes < filestat.st_size)
    {
        ssize_t remaining_bytes = filestat.st_size - sent_bytes;
        ssize_t bytes_sent = sendfile(client_socket, filefd, &offset, remaining_bytes);
        if (bytes_sent <= 0)
        {
            perror("sendfile error");
            break;
        }
        sent_bytes += bytes_sent;
    }

    close(filefd);
}

void send_php(int client_socket, const char *filename)
{
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    char command[512];

    sprintf(command, "php %s", filename);
    FILE *php_output = popen(command, "r");

    if (php_output == NULL)
    {
        perror("PHP execution error");
        close(client_socket);
        return;
    }

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, php_output)) > 0)
    {
        send(client_socket, buffer, bytesRead, 0);
    }

    pclose(php_output);
}

int main()
{
    // store the file descriptors (identifiers) for the server socket and the new client socket
    // A file descriptor is a unique identifier for an open socket
    int server_fd, new_socket;
    // used to represent an IPv4 socket address
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // AF_INET means IPv4 address family
    // SOCK_STREAM = indicate TCP socket
    // 0 = OS select an appropriate protocol based on the provided features
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
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

    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening for connections\n");

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Accept the connection\n");

        ssize_t bytesRead = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
        {
            perror("Receive error");
            close(new_socket);
            continue;
        }

        buffer[bytesRead] = '\0';

        if (strstr(buffer, "POST /submit") != NULL)
        {
            const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            send(new_socket, response, strlen(response), 0);

            // Process the form data
            char *data_start = strstr(buffer, "\r\n\r\n") + 4; // Find the start of POST data
            char *current_field = strtok(data_start, "&");
            char response_page[BUFFER_SIZE];

            snprintf(response_page, sizeof(response_page), "<html><body><h1>Form Data</h1>");

            while (current_field != NULL)
            {
                char field_name[256];
                char field_value[256];

                sscanf(current_field, "%[^=]=%s", field_name, field_value);

                // URL decode the field name and value (optional)
                // Implement URL decoding here if needed

                snprintf(response_page + strlen(response_page), sizeof(response_page) - strlen(response_page),
                         "<p>%s: %s</p>", field_name, field_value);

                current_field = strtok(NULL, "&");
            }

            snprintf(response_page + strlen(response_page), sizeof(response_page) - strlen(response_page),
                     "</body></html>");

            send(new_socket, response_page, strlen(response_page), 0);

            close(new_socket);
        }
        else
        {
            char filename[256];
            char filepath[512];

            sscanf(buffer, "GET /%255s", filename);

            if (!strcmp(filename, "HTTP/1.1"))
            {
                strcpy(filename, "index.html");
            }

            sprintf(filepath, "www/%s", filename);

            // Determine content type
            const char *content_type = "text/html"; // Default to plain text

            if (strstr(filepath, ".pdf") != NULL)
            {
                content_type = "application/pdf";
            }
            else if (strstr(filepath, ".jpg") != NULL || strstr(filepath, ".jpeg") != NULL)
            {
                content_type = "image/jpeg";
            }

            if (access(filepath, F_OK) == 0)
            {
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
                send(new_socket, response, strlen(response), 0);

                if (strstr(filename, ".php") != NULL)
                {
                    send_php(new_socket, filepath);
                }
                else
                {
                    send_regular(new_socket, filepath);
                }
            }
            else
            {
                content_type = "text/html";
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
                send(new_socket, response, strlen(response), 0);

                send_regular(new_socket, "www/error.html");
            }

            printf("Response sent to client\n\n");
            close(new_socket);
        }
    }

    return 0;
}