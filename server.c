#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 2854
#define BUFFER_SIZE 1024
void error(char *msg);

int main() {
    // create socket
    // server_socket: socket descriptor, an integer (like a file-handle)
    // AF_INET: IPv4, SOCK_STREAM: TCP(reliable, connection oriented), protocol is zero, meaning the operating system will choose the most appropriate protocol.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        error("Error: create socket failed");
    }

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

    // Listen for connections
    if (listen(server_socket, 20) < 0) {
        error("Error: listen failed");
    }

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
                else {
                    printf("Client: %s from %s:%d\n", buffer, inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
                    send(new_socket, buffer, sizeof(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }
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