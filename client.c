#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 2854
#define BUFFER_SIZE 1024
void error(char *msg);
void *ReceiveThread(void *sock_fd);

int main(){
    // Create a socket for the client
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        error("Error: create socket failed");
    }

    // Name the socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  //每个字节都用0填充
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    server_addr.sin_port = htons(PORT); // port number
    // if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
    //     error("Error: connect failed");
    // }
    int connected = 0;

    // transfer    
    char buffer[BUFFER_SIZE];
    while(1) {
        printf("Client:\n");
        printf("[1] Connect\n");
        printf("[2] Disconnect\n");
        printf("[3] Get time\n");
        printf("[4] Get host name\n");
        printf("[5] Get activitiy link list\n");
        printf("[6] Send message\n");
        printf("[7] Exit\n");
        printf("Select a number above: \n");

        scanf("%s", buffer);
        // send(client_socket, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "1") == 0) {
            // have connected
            if (connected == 1) {
                printf("Client: Have connected\n");
            }
            // have not connected
            // connect failed
            else if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
                close(client_socket);
                // error("Error: connect failed");
                printf("Client: Connect failed");
            }
            // connect succeed
            else {
                connected = 1;
                pthread_t tid;

                // create the thread to receive data
                // if (pthread_create(&tid, NULL, ReceiveThread, NULL) != 0) {
                if (pthread_create(&tid, NULL, ReceiveThread, &client_socket) != 0) {
                    connected = 0;
                    close(client_socket);
                    error("Error: client create thread failed\n");
                }
                else {
                    printf("Server: Connection succeed\n");
                }
            }
        }
        if (strcmp(buffer, "2") == 0) {
            if (connected == 1) {
                connected = 0;
                printf("Server: disconnected and exit\n");
                close(client_socket);
                break;
            }
            else {
                printf("No connection\n");
            }
        }
        if (strcmp(buffer, "3") == 0) {
            if (connected == 1) {
                char *request_packet = (char*)malloc(sizeof(int) + 4 * sizeof(char));
                *request_packet = '$';
                *(request_packet + 1) = 'Q';
                *(request_packet + 2) = 'T';
                *(int*)(request_packet + 3) = -1;
                *((char*)((int *)(request_packet + 3) + 1)) = '$';
                if (send(client_socket, request_packet, sizeof(int) + 4 * sizeof(char), 0) < 0) {
                    perror("Send time request failed\n");
                }
                // wait for ReceiveThread 

            }
            else {
                printf("No connection\n");
            }
        }
        if (strcmp(buffer, "4") == 0) {
            if (connected == 1) {
                char *request_packet = (char*)malloc(sizeof(int) + 4 * sizeof(char));
                *request_packet = '$';
                *(request_packet + 1) = 'Q';
                *(request_packet + 2) = 'N';
                *(int*)(request_packet + 3) = -1;
                *((char*)((int *)(request_packet + 3) + 1)) = '$';
                if (send(client_socket, request_packet, sizeof(int) + 4 * sizeof(char), 0) < 0) {
                    perror("Send host name request failed\n");
                }
                // wait for ReceiveThread 
            }
            else {
                printf("No connection\n");
            }
        }
        if (strcmp(buffer, "5") == 0) {
            if (connected == 1) {
                char *request_packet = (char*)malloc(sizeof(int) + 4 * sizeof(char));
                *request_packet = '$';
                *(request_packet + 1) = 'Q';
                *(request_packet + 2) = 'L';// send client list
                *(int*)(request_packet + 3) = -1;
                *((char*)((int *)(request_packet + 3) + 1)) = '$';
                if (send(client_socket, request_packet, sizeof(int) + 4 * sizeof(char), 0) < 0) {
                    perror("Send client list request failed\n");
                }
                // wait for ReceiveThread
            }
            else {
                printf("No connection\n");
            }
        }
        if (strcmp(buffer, "6") == 0) {
            if (connected == 1) {
                char *request_packet = (char*)malloc(sizeof(int) + 4 * sizeof(char));
                *request_packet = '$';
                *(request_packet + 1) = 'Q';
                *(request_packet + 2) = 'M';// send message
                *(int*)(request_packet + 3) = -1;
                *((char*)((int *)(request_packet + 3) + 1)) = '$';
                if (send(client_socket, request_packet, sizeof(int) + 4 * sizeof(char), 0) < 0) {
                    perror("Send message request failed\n");
                }
                // wait for ReceiveThread 
            }
            else {
                printf("No connection\n");
            }
        }
        if (strcmp(buffer, "7") == 0) {
            connected = 0;
            close(client_socket);
            printf("Exit\n");
            // exit(1);
            break;// jump to while loop
        }

        // if (recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
        //     error("Error: receive data failed");
        // }
        // else {
        //     printf("Server: %s\n", buffer);
        // }
    }

    return 0;
}

void error(char *msg) {
    perror(msg);
    exit(1);
}

// This will handle connection for each client
void *ReceiveThread(void *sock_fd) {
	// Get the socket descriptor
	int conn_id = *(int*)sock_fd;
	
	// request data
	char request[BUFFER_SIZE];
	
	// response data
	char response[BUFFER_SIZE];
	
	// read response continue
	while (1) {
        // judge if a complete response packet
        if (recv(conn_id, response, BUFFER_SIZE, 0) > 0) {
            printf("Client: receive: %s\n", request);
            if (*response == '$' && *(response + 1) == 'R' && *((char*)((int*)(response + 3) + 1)) == '$') {
                if (*(response + 2) == 'T') {
                    int lengh = *(int*)(response + 3);
                    char *host_time = (char*)malloc(lengh * sizeof(char) - sizeof(char) * 4 - sizeof(int) + sizeof(char));
                    strcpy(host_time, (char*)((int *)(response + 3) + 1) + 1);
                    *(host_time + lengh - 8) = 0;
                    printf("Client get response successfully, time is %s\n", host_time);
                    free(host_time);
                }
                else if (*(response + 2) == 'N') {
                    // char host_name[BUFFER_SIZE];
                    // gethostname(host_name, sizeof(host_name));
                    // // build response packet
                    // char *response = (char*)malloc(sizeof(char) * 4 + sizeof(int) + sizeof(char) * 1024);
                    // memset(response, 0, sizeof(char) * 4 + sizeof(int) + sizeof(char) * 1024);
                    // *response = '$';
                    // *(response + 1) = 'R';
                    // *(response + 2) = 'N';
                    // int length = (int)(strlen(host_name) + sizeof(char) * 4 + sizeof(int));
                    // printf("length = %d\n", length);// debug
                    // // *(int*)(response + 3) = length;
                    // *(int*)(response + 3) = -1;// bug
                    // *((char*)((int *)(response + 3) + 1)) = '$';
                    // strcat((char*)((int *)(response + 3) + 1), host_name);

                    // if (send(conn_id, response, length, 0) > 0) {
                    //     printf("Server send: %s\n", response);
                    // }
                    // // char *p = response;
                    // // while (length > 0) {
                    // //     send(conn_id, p, length > BUFFER_SIZE ? BUFFER_SIZE : length, 0);
                    // //     length -= BUFFER_SIZE;
                    // //     p += BUFFER_SIZE;
                    // // }
                    // free(response);
                }
                else if (*(response + 2) == 'L') {

                }
                else if (*(response + 2) == 'M') {

                }
            }
        }
		// clear request data
		memset(request, 0, BUFFER_SIZE);
		
		// // send response
		// if (send(conn_id, response, strlen(response), 0) > 0) {
		// 	printf("SEND: %s\n", response);
		// } else {
		// 	perror("send message error\n");
		// }
	}
	
	// // terminate connection
	// close(conn_id);
	// std::cout << "[INFO] CONNECTION CLOSED\n";
	
	// // decrease connection counts
	// connection--;
	
	// // thread automatically terminate after exit connection handler
	// std::cout << "[INFO] THREAD TERMINATED" << std::endl;
	
	// delete (int*)sock_fd;
	
	// // print line
	// std::cout << "------------------------" << std::endl;
	
	// exiting
	pthread_exit(NULL);
} 