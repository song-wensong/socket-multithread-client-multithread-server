#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#include <time.h>

#define PORT 2854
#define BUFFER_SIZE 1024
// maximum connection requests queued
#define QUEUE_CONNECTION 5

void error(char *msg);
int en_queue(struct sockaddr_in *addr, int front, int rear, struct sockaddr_in new_addr);
int de_queue(struct sockaddr_in *addr, int front, int rear);
void *HandleThread(void *sock_fd);

int main() {
    // Create an unnamed socket for the server
    // AF_INET: IPv4, SOCK_STREAM: TCP(reliable, connection oriented), protocol is zero, meaning the operating system will choose the most appropriate protocol.
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        error("Error: create socket failed");
    }
    printf("Server: Create socket succeed\n");

    // Name the socket
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
    struct sockaddr_in addr[QUEUE_CONNECTION + 1];
    int front = 0, rear = 0;

    struct sockaddr_in new_addr;
    socklen_t new_addr_size = sizeof(new_addr);
    int new_socket;

    // send and receive data
    char buffer[BUFFER_SIZE];
    // pid_t childPid;
    while (1) {
        new_socket = accept(server_socket, (struct sockaddr*)&new_addr, &new_addr_size);
        if (new_socket < 0) {
            error("Error: accept failed");
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
        rear = en_queue(addr, front, rear, new_addr);

        pthread_t tid;
        if (pthread_create(&tid, NULL, HandleThread, &new_socket) != 0) {
            close(new_socket);
            error("Error: client create thread failed\n");
        }
        else {
            printf("Server: Connection succeed\n");
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

int en_queue(struct sockaddr_in *addr, int front, int rear, struct sockaddr_in new_addr) {
    if ((rear + 1) % (QUEUE_CONNECTION + 1) ==front) {
        perror("queue is full\n");
        return rear;
    }
    addr[rear % (QUEUE_CONNECTION + 1)] = new_addr;
    // rear++;
    rear = (rear + 1) % (QUEUE_CONNECTION + 1);
    return rear;
}

int de_queue(struct sockaddr_in *addr, int front, int rear) {
    if(front == rear % (QUEUE_CONNECTION + 1)) {
        perror("Error: queue is empty\n");
        return front;
    }
    // printf("%d ",a[front]);

    front = (front + 1) % (QUEUE_CONNECTION + 1);
    return front;
}


// This will handle connection for each client
void *HandleThread(void *sock_fd) {
	// Get the socket descriptor
	int conn_id = *(int*)sock_fd;
	
	// request data
	char receive_packet[BUFFER_SIZE];
	
	// // response data
	// char response[BUFFER_SIZE];
	
	// read response continue
	while (1) {
		memset(receive_packet, 0, sizeof(receive_packet));
        if (recv(conn_id, receive_packet, BUFFER_SIZE, 0) > 0) {
            // judge if a complete response packet
            if (*receive_packet == '$' && *(receive_packet + 1) == 'Q' && *((char*)((int*)(receive_packet + 3) + 1)) == '$') {
                if (*(receive_packet + 2) == 'T') {
                    time_t *timep = malloc(sizeof(time_t));
                    time(timep);
                    char *host_time = ctime(timep);

                    // build response packet
                    char *response = (char*)malloc(sizeof(char) * 4 + sizeof(int) + sizeof(char) * 1024);
                    memset(response, 0, sizeof(char) * 4 + sizeof(int) + sizeof(char) * 1024);
                    *response = '$';
                    *(response + 1) = 'R';
                    *(response + 2) = 'T';
                    int length = strlen(host_time) + sizeof(char) * 4 + sizeof(int);
                    // int length = strlen(host_time) + ((char*)((int *)(response + 3) + 2)) ;
                    // int length = strlen(host_time);
                    // *(int*)(response + 3) = length;
                    printf("length = %d\n", length);// debug
                    // int test = -1;
                    // *(int*)(response + 3) = -1;// bug
                    // *(int*)(response + 3) = test;// bug
                    *(int*)(response + 3) = length;
                    *((char*)((int *)(response + 3) + 1)) = '$';
                    strcat((char*)((int *)(response + 3) + 1), host_time);

                    if (send(conn_id, response, length, 0) > 0) {
                        // printf("Server send: %s\n", response);
                        for (int i = 0; i < length; i++) {
                            printf("%c ", *(response + i));
                        }
                        printf("length = %d\n", length);
                        printf("\n");
                        printf("Server send: %s\n", response);
                    }
                    free(response);
                }
                else if (*(receive_packet + 2) == 'N') {
                    char host_name[BUFFER_SIZE];
                    gethostname(host_name, sizeof(host_name));
                    // build response packet
                    char *response = (char*)malloc(sizeof(char) * 4 + sizeof(int) + sizeof(char) * 1024);
                    memset(response, 0, sizeof(char) * 4 + sizeof(int) + sizeof(char) * 1024);
                    *response = '$';
                    *(response + 1) = 'R';
                    *(response + 2) = 'N';
                    int length = (int)(strlen(host_name) + sizeof(char) * 4 + sizeof(int));
                    printf("length = %d\n", length);// debug
                    // *(int*)(response + 3) = length;
                    *(int*)(response + 3) = -1;// bug
                    *((char*)((int *)(response + 3) + 1)) = '$';
                    strcat((char*)((int *)(response + 3) + 1), host_name);

                    if (send(conn_id, response, BUFFER_SIZE, 0) > 0) {// some improvement
                        printf("Server send: %s\n", response);
                    }
                    // char *p = response;
                    // while (length > 0) {
                    //     send(conn_id, p, length > BUFFER_SIZE ? BUFFER_SIZE : length, 0);
                    //     length -= BUFFER_SIZE;
                    //     p += BUFFER_SIZE;
                    // }
                    free(response);
                }
                else if (*(receive_packet + 2) == 'L') {

                }
                else if (*(receive_packet + 2) == 'M') {

                }
            }
            
            
            // // send response
            // if (send(conn_id, receive_packet, strlen(response), 0) > 0) {
            //     printf("SEND: %s\n", receive_packet);
            // }
            // else {
            //     perror("send message error\n");
            // }
            bzero(receive_packet, sizeof(receive_packet));
        }
	}
	
	// // terminate connection
	// close(conn_id);
	// std::cout << "[INFO] CONNECTION CLOSED\n";
	
	// // decrease connection counts
	// connection--;
	
	// // thread automatically terminate after exit connection handler
	// std::cout << "[INFO] THREAD TERMINATED" << std::endl;
	
	close(conn_id);
	
	// // print line
	// std::cout << "------------------------" << std::endl;
	
	// exiting
	pthread_exit(NULL);
} 