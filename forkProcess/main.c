#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    pid_t result;

    pid_t pid, child_pid;
    int status;

    printf("This is a system demo \n");

    result = system("ls -l");

    result = fork();
    if(result == -1){
        printf("fork error\n");
    } else if(result == 0){
        printf("the returned value is %d, in child process my PID is %d\n", result, getpid());
    }

#if 0
    // 这段代码会影响后面的执行，execl 相当于新建了一个fork
    result = execl("/bin/ls", "ls", "-al", NULL);
    if(result != 0){
        printf("ls ERROR\n");
    }
#endif
    pid = fork();
    if(pid < 0){
        printf("fork error \n");
    } else if(pid == 0){
        printf("I am a child process y pid is %d\n", getpid());

        sleep(3);

        printf("I am about to quit the process \n");

        exit(0);
    } else {
        child_pid = wait(&status);
        if(child_pid == pid){
            printf("get exit child process id %d\n", child_pid);
            printf("get chile exit status %d\n", status);
        } else {
            printf("Error\n");
        }
        exit(0);
    }

    printf("Done \n\n");

    return result;
}
