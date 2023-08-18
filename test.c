#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to send a file to the client
void send_file(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File open error");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytesRead, 0);
    }

    fclose(file);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind error");
        exit(1);
    }

    // Listen
    if (listen(server_socket, 10) == -1) {
        perror("Listen error");
        exit(1);
    }

    while (1) {
        // Accept connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accept error");
            exit(1);
        }

        // Read client request
        char request[BUFFER_SIZE];
        ssize_t bytesRead = recv(client_socket, request, sizeof(request), 0);
        if (bytesRead <= 0) {
            perror("Receive error");
            close(client_socket);
            continue;
        }

        // Null-terminate the request
        request[bytesRead] = '\0';

        // Parse request to get the filename
        char filename[256];
        sscanf(request, "GET /%255s", filename);

        // Open the requested file
        char filepath[256];
        sprintf(filepath, "www/%s", filename);

        // int filefd = open(filepath, O_RDONLY);
        // if (filefd == -1) {
        //     perror("File open error");
        //     close(client_socket);
        //     continue;
        // }

        // Send HTTP response headers
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(client_socket, response, strlen(response), 0);

        // Send file content to client
        send_file(client_socket, filepath);

        // Close the file and client socket
        // close(filefd);
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
