#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


#define HOST "10.168.1.185"
#define PORT 11111
#define BUFFER_SIZE (4 * 1024)

int main(){
    int sockfd;

    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("create socekt endpoint failed\n");
        exit(-1);
    }

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(HOST);

    if(connect(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1){
        printf("connect server failed\n");
        close(sockfd);
        exit(-1);
    }

    printf("connect server successs\n");

    while(1){
        printf("please enter some test:");
        fgets(buffer, BUFFER_SIZE, stdin);

        if(strncmp(buffer, "end", 3) == 0){
            break;
        }
        write(sockfd, buffer, sizeof(buffer));
    }

    close (sockfd);
    exit(0);
}
