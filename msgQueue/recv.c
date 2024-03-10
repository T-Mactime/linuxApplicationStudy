#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>

#define MAX_LEN 512

typedef struct message {
    long type;
    char text[MAX_LEN];
} message_t;

int main(){
    int qid;
    message_t msg;

    if((qid = msgget((key_t)1234, IPC_CREAT|0666)) == -1){
        perror("mesget");
        exit(1);
    }
    printf("open queue %d\n", qid);

    do {
        memset(msg.text, 0x00, sizeof(msg.text));

        if(msgrcv(qid, (void*)&msg, MAX_LEN, 0,0 ) < 0){
            perror("msgrcv");
            exit(0);
        }

        printf("the message from process %ld %s", msg.type, msg.text);
    } while(strncmp(msg.text, "quit", 4));

    if((msgctl(qid, IPC_RMID, NULL)) < 0){
        perror("msgctl");
        exit(1);
    } else {
        printf("delete msg qid %d\n", qid);
    } 

    exit(0);
}