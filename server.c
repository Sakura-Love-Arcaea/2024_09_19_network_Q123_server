#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SOCKET_CLOSED 0
#define SHUT_DOWN -1

typedef struct sockaddr_in sockaddr_in;
typedef void (*messageHandler)(char* message, sockaddr_in* client, int client_socket_file_descriptor);

int isCommand(char* buffer) {
    return buffer[0] == '/';
}

void plus_OK(char* buffer, sockaddr_in* client, int client_socket_file_descriptor) {
    char reply[256] = "no reply request\n";
    strcat(buffer, " OK\n");
    printf("%s:%d said: %s", inet_ntoa(client->sin_addr), ntohs(client->sin_port), buffer);
    send(client_socket_file_descriptor, reply, strlen(reply), 0);
}

void countLetter(char* buffer, sockaddr_in* client, int client_socket_file_descriptor) {
    char reply[256];
    int  letter_count = 0;
    int  number_count = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            number_count++;
        } else if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z')) {
            letter_count++;
        }
    }
    printf("%s:%d said: %s\n", inet_ntoa(client->sin_addr), ntohs(client->sin_port), buffer);
    sprintf(reply, "there are %d letters and %d numbers in the string", letter_count, number_count);
    send(client_socket_file_descriptor, reply, strlen(reply), 0);
}

void shiftLetter_Left(char* buffer, sockaddr_in* client, int client_socket_file_descriptor) {
    char reply[256];
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            buffer[i] = (buffer[i] - 'a' + 1) % 26 + 'a';
        } else if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            buffer[i] = (buffer[i] - 'A' + 1) % 26 + 'A';
        }
    }
    printf("%s:%d said: %s\n", inet_ntoa(client->sin_addr), ntohs(client->sin_port), buffer);
    sprintf(reply, "shifted string: %s", buffer);
    send(client_socket_file_descriptor, reply, strlen(reply), 0);
}

void homeWork(char* buffer, sockaddr_in* client, int client_socket_file_descriptor) {
    char reply[256];
    int  letter_count = 0;
    int  number_count = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9') {
            number_count++;
        } else if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z')) {
            letter_count++;
        }
    }
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            buffer[i] = (buffer[i] - 'a' - 1) % 26 + 'a';
        } else if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            buffer[i] = (buffer[i] - 'A' - 1) % 26 + 'A';
        }
    }
    char* p = NULL;
    for (int i = 0; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            p = &buffer[i];
            break;
        }
    }
    if (p == NULL) {
        p = "no letter in the string";
    }
    printf("%s:%d said: %s\n", inet_ntoa(client->sin_addr), ntohs(client->sin_port), buffer);
    sprintf(reply, "%s", p);
    send(client_socket_file_descriptor, reply, strlen(reply), 0);
}
messageHandler tasks[] = {
    plus_OK,
    countLetter,
    shiftLetter_Left,
    homeWork,
};

int startRecv(int client_socket_file_descriptor, sockaddr_in* client, messageHandler messageHandler) {
    char buf[256] = {};
    int  n        = 0;
    for (;;) {
        n = recv(client_socket_file_descriptor, buf, sizeof(buf), 0);
        if (n == 0) {
            return SOCKET_CLOSED;
        }
        if (strchr(buf, '\n')) {
            strchr(buf, '\n')[0] = '\0';
        }
        if (isCommand(buf)) {
            if (strcmp(buf, "/shutDown") == 0) {
                return SHUT_DOWN;
            }
        }
        messageHandler(buf, client, client_socket_file_descriptor);
        memset(buf, 0, sizeof(buf));
    }
}

int main() {
    int         server_socket_file_descriptor, client_socket_file_descriptor;
    sockaddr_in server, client;
    socklen_t   server_len = sizeof(server), client_len = sizeof(client);

    // 初始化
    bzero(&server, sizeof(sockaddr_in));
    server.sin_family      = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port        = ntohs(8080);

    server_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(server_socket_file_descriptor, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Bind failed\n");
        close(server_socket_file_descriptor);
        exit(1);
    }
    printf("SERVER: socket binded\n");
    if (listen(server_socket_file_descriptor, 5) < 0) {
        printf("Listen failed\n");
        close(server_socket_file_descriptor);
        exit(1);
    }
    printf("SERVER: Listening on port %d...\n", htons(server.sin_port));

    while (1) {
        client_socket_file_descriptor = accept(server_socket_file_descriptor, (struct sockaddr*)&client, &client_len);
        printf("SERVER: Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        int status = startRecv(client_socket_file_descriptor, &client, tasks[3]);

        switch (status) {
            case SOCKET_CLOSED:
                printf("SERVER: Connection closed by client\n");
                continue;
            case SHUT_DOWN:
                printf("SERVER: shut down\n");
                goto close;
        }
    }

close:
    send(client_socket_file_descriptor, "/ServerShutDown", strlen("/ServerShutDown"), 0);
    close(server_socket_file_descriptor);
    return 0;
}
