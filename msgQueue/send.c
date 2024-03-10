#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

typedef struct message
{
    /* data */
    long msg_type;
    char msg_text[BUFFER_SIZE];
} message_t;

int main()
{
    int qid;

    message_t msg;

    if ((qid = msgget((key_t)1234, IPC_CREAT | 0666)) == -1)
    {
        perror("message\n");
        exit(1);
    }

    printf("open queue %d\n", qid);

    while (1)
    {
        printf("enter some message to the queue:");

        if ((fgets(msg.msg_text, BUFFER_SIZE, stdin)) == NULL)
        {
            printf("get message end\n");
            exit(1);
        }

        msg.msg_type = getpid();

        if ((msgsnd(qid, &msg, strlen(msg.msg_text), 0)) < 0)
        {
            perror("send message error\n");
            exit(1);
        }

        else
        {
            printf("send message \n");
        }
        if (strncmp(msg.msg_text, "quit", 4) == 0)
        {
            printf("quit get message \n");
            break;
        }
    }

    exit(0);
}