#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 2854
#define BUFFER_SIZE 1024
void error(char *msg);

int main(){
    //create socket, protocol is zero, meaning the operating system will choose the most appropriate protocol.
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        error("Error: create socket failed");
    }

    // connect
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  //每个字节都用0填充
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    server_addr.sin_port = htons(PORT); // port number
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
        error("Error: connect failed");
    }

    // transfer    
    char buffer[BUFFER_SIZE];
    while(1) {
        printf("[Client]: ");
        scanf("%s", buffer);
        send(client_socket, buffer, sizeof(buffer), 0);
        // write(client_socket, buffer, sizeof(buffer));
        
        if (strcmp(buffer, "exit") == 0) {
            close(client_socket);
            printf("Exit\n");
            exit(1);
        }
        if (recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
            error("Error: receive data failed");
        }
        else {
            printf("Server: %s\n", buffer);
        }
    }

    return 0;
}

void error(char *msg) {
    perror(msg);
    exit(1);
}