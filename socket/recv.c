#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX (4 * 1024)
#define PORT 11111

int main()
{
    char buf[MAX];
    // int n;
    int sockfd, connfd;
    uint len;

    struct sockaddr_in server, client;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("create socket failed\n");
        exit(-1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(sockfd, (struct sockaddr *)&server, sizeof(server))) != 0)
    {
        printf("socket bind failed\n");
        exit(-1);
    }

    if ((listen(sockfd, 5)) != 0)
    {
        exit(0);
    }

    connfd = accept(sockfd, (struct sockaddr *)&client, &len);
    if (connfd < 0)
    {
        printf("error");
        exit(-1);
    }

    while (1)
    {
        bzero(buf, MAX);

        if (read(connfd, buf, sizeof(buf)) <= 0)
        {
            printf("client close\n");
            close(connfd);
            break;
        }

        printf("recv buf is : %s\n", buf);

        if (strncmp("end", buf, 3) == 0)
        {
            close(connfd);
            break;
        }
    }
    close(sockfd);
    exit(0);
}