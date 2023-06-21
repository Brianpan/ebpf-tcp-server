#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 12345
#define BUF_SIZE 1024

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char **argv)
{
    struct sockaddr_in addr = {
        .sin_family = PF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    socklen_t socklen = sizeof(addr);
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);
    int sock_fd;
    if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        error("Fail to create socket.");
  
    if (bind(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        error("Fail to bind.");
  
    if (listen(sock_fd, 1024) < 0)
        error("Fail to listen");

    while (1) {
        int client = accept(sock_fd, (struct sockaddr *) &client_addr, &clientlen);
        if (client < 0)
            error("accept");
        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
    }

    close(sock_fd);

    return 0;
}
