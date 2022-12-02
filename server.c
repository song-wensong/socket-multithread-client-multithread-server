#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 2854
#define BUFFER_SIZE 1024
// maximum connection requests queued
#define QUEUE_CONNECTION 20

void error(char *msg);

int main() {
    // create socket
    // server_socket: socket descriptor, an integer (like a file-handle)
    // AF_INET: IPv4, SOCK_STREAM: TCP(reliable, connection oriented), protocol is zero, meaning the operating system will choose the most appropriate protocol.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        error("Error: create socket failed");
    }
    printf("Server: Create socket succeed\n");

    // Bind the socket to an address using the bind() system call.
    // For a server socket on the Internet, an address consists of a port number on the host machine.
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  // IPv4
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT); // port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error("Error: bind failed");
    }
    printf("Server: bind succeed\n");

    // Listen for connections
    if (listen(server_socket, QUEUE_CONNECTION) < 0) {
        error("Error: listen failed");
    }
    printf("Server: listen succeed\n");

    // Accept a connection 
    struct sockaddr_in new_addr;
    socklen_t new_addr_size = sizeof(new_addr);
    int new_socket;
    
    // send and receive data
    char buffer[BUFFER_SIZE];
    pid_t childPid;
    while (1) {
        new_socket = accept(server_socket, (struct sockaddr*)&new_addr, &new_addr_size);
        if (new_socket < 0) {
            error("Error: accept failed");
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
        if ((childPid = fork()) == 0) {
            while (1) {
                if (recv(new_socket, buffer, sizeof(buffer), 0) < 0) {
                    error("Error: receive data failed");
                    break;
                }
                printf("Client: %s from %s:%d\n", buffer, inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
                
                if (strcmp(buffer, "3") == 0) {
                    time_t seconds;
                    seconds = time(NULL);
                    // itoa(seconds/3600, buffer, 10);
                    sprintf(buffer, "%ld", seconds/3600);
                    printf("自 1970-01-01 起的小时数 = %ld\n", seconds/3600);
                }
                if (strcmp(buffer, "4") == 0) {
                    char server_name[256];
                    gethostname(server_name, sizeof(server_name));
                    strcpy(buffer, server_name);
                    printf("server_name = %s\n", server_name);
                }
                send(new_socket, buffer, sizeof(buffer), 0);
                bzero(buffer, sizeof(buffer));
            }
        }
    }
   
    // close
    close(new_socket);
    close(server_socket);

    return 0;
}

void error(char *msg) {
    perror(msg);
    exit(1);
}