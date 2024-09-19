#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define Q3



int main() {
    struct sockaddr_in server, client;
    int sock_server, sock_client;
    int read_size;
    int addr_len = sizeof(struct sockaddr_in);
    char buffer[1024] = {0};

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8888);
    sock_server = socket(AF_INET, SOCK_STREAM, 0);



    bind(sock_server, (struct sockaddr *)&server, sizeof(server));
    sock_client = accept(sock_server, (struct sockaddr *)&client, &addr_len);
    listen(sock_server, 5);


    addr_len = sizeof(struct sockaddr_in);
    sock_client = accept(sock_server, (struct sockaddr *)&client, &addr_len);

    #ifdef Q1
    read_size = recv(sock_client, buffer, sizeof(buffer)-1, 0);
    int letter_count = 0;
    int number_count = 0;
    printf("Received_raw: %s\n", buffer);
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            number_count++;
        } else if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z')) {
            letter_count++;
        }
    }
    printf("there are %d letters and %d numbers in the string\n", letter_count, number_count);
    
    #endif

    #ifdef Q2
    read_size = recv(sock_client, buffer, sizeof(buffer)-1, 0);
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == 'Z') {
            buffer[i] = 'A';
        } else if (buffer[i] == 'z') {
            buffer[i] = 'a';
        } else if ((buffer[i] >= 'A' && buffer[i] < 'Z') || (buffer[i] >= 'a' && buffer[i] < 'z')) {
            buffer[i] = buffer[i] + 1;
        }
    }
    printf("Received: %s\n", buffer);
    #endif
    
    
    #ifdef Q3
    while (1) {
        read_size = recv(sock_client, buffer, sizeof(buffer)-1, 0);
        if (read_size > 0) {
            printf("keep alive\n");
        } else if (read_size == 0) {
            printf("Client has closed the disconnected\n");
            exit(1);
        }
    }
    
    #endif
    close(sock_client);
    close(sock_server);
}
