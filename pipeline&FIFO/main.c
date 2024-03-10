#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

#define MAX_DATA_LEN 256
#define DELAY_TIME 1

#define MYFIFO "myfifo"
#define MAX_BUFF_SIZE PIPE_BUF

void pipeTest(){
    pid_t pid;
    int pipe_fd[2];

    char buf[MAX_DATA_LEN];
    const char data[] = "pipe test program";

    int real_read, real_write;

    memset(buf, 0x00, sizeof(buf));

    if (pipe(pipe_fd) < 0)
    {
        exit(1);
    }
    if ((pid = fork()) == 0)
    {
        close(pipe_fd[1]);
        sleep(DELAY_TIME * 3);
        if ((real_read = read(pipe_fd[0], buf, MAX_DATA_LEN)) > 0)
        {
            printf("%d bytes read from the pipe is '%s'\n", real_read, buf);
        }
        close(pipe_fd[0]);
        exit(0);
    }
    else if (pid > 0)
    {
        close(pipe_fd[0]);
        sleep(DELAY_TIME);
        if ((real_write = write(pipe_fd[1], data, strlen(data)) != -1))
        {
            printf("parent write %d type: '%s'\n", real_write, data);
        }
        close(pipe_fd[1]);
        exit(0);
    }
}

void fifo_read(){
    char buf[MAX_BUFF_SIZE];
    int fd;
    int nread;

    printf("**********************read info*********************\n");

    if(access(MYFIFO, F_OK) == -1){
        if((mkfifo(MYFIFO, 0666) < 0) &&(errno != EEXIST)){
            printf(" connot create fifo file\n");
            exit(1);
        }
    }

    fd = open(MYFIFO, O_RDONLY);
    if(fd == -1){
        printf("open fifo file error\n");
        exit(1);
    }

    memset(buf, 0, sizeof(buf));

    if((nread = read(fd, buf, MAX_BUFF_SIZE)) > 0){
        printf("read '%s' from FIFO\n", buf);
    }

    printf("******************close fifo*****************\n");
    close(fd);

    exit(0);
}

void fifo_write(){
    int fd;
    char buf[] = "this is a fifo test demo ";
    int nwrite;

    sleep (2);

    fd = open(MYFIFO, O_WRONLY|O_CREAT, 0644);
    if(fd == -1){
        printf("open fifo file error \n");
        exit(1);
    }
    printf("write '%s' to fifo\n", buf);

    nwrite = write(fd, buf, MAX_BUFF_SIZE);

    if(wait(NULL)){
        close(fd);
        exit(0);
    }
}

void fifoTest(){
    pid_t result;
    result = fork();

    if(result == -1){
        printf("fork error\n");
    }

    else if(result == 0){
        fifo_read();
    } else {
        fifo_write();
    }
}

int main()
{
    // pipeTest();

    fifoTest();

    return 0;
}