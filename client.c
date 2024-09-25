#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

char buf[256] = {};

void before_send(char* buffer) {
    for (int i = 0; i < strlen(buffer); i++) buffer[i] = toupper(buffer[i]);
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            buffer[i] = (buffer[i] - 'a' + 1) % 26 + 'a';
        } else if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
            buffer[i] = (buffer[i] - 'A' + 1) % 26 + 'A';
        }
    }
}

int isCommand(char* buffer) {
    return buffer[0] == '/';
}

int main() {
    int client_sfd;
    client_sfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in target_server;
    bzero(&target_server, sizeof(target_server));
    target_server.sin_family      = PF_INET;
    target_server.sin_addr.s_addr = inet_addr("127.0.0.1");
    target_server.sin_port        = htons(8080);

    connect(client_sfd, (struct sockaddr*)&target_server, sizeof(target_server));
    printf("Connected to the server %s:%d\n", inet_ntoa(target_server.sin_addr), htons(target_server.sin_port));

    while (1) {
        printf("Enter message: ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        if (!isCommand(buf)) before_send(buf);
        printf("confrim: %s", buf);
        send(client_sfd, buf, sizeof(buf), 0);
        memset(buf, 0, sizeof(buf));

        recv(client_sfd, buf, sizeof(buf), 0);
        if (strchr(buf, '\n')) {
            strchr(buf, '\n')[0] = '\0';
        }

        if (strcmp(buf, "/ServerShutDown") == 0) {
            break;
        }

        printf("Server reply: %s\n", buf);
        memset(buf, 0, sizeof(buf));
    }
end:
    close(client_sfd);
}
