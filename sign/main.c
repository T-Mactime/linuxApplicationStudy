#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void single_handle(int sig)
{
    printf("thisl signal number is %d\n", sig);

    if (sig == SIGINT)
    {
        printf("I have get sigint \n");
        signal(SIGINT, SIG_DFL);
    }
}

int main()
{

#if 0
    printf("this is an singal test function\n");


    signal(SIGINT, single_handle);

    while(1){
        printf("waiting for the sigint ^c,\n");

        sleep(1);
    }

    _exit(0);
    return 0;
#endif

#if 0
    // 信号
    struct sigaction act;
    printf("This is sigactoin function test demo\n");
    act.sa_handler = single_handle;

    sigemptyset(&act.sa_mask);

    act.sa_flags = SA_RESETHAND;

    sigaction(SIGINT, &act, NULL);

    while (1)
    {
        printf("waiting for the sigint ^c,\n");

        sleep(1);
    }

    _exit(0);

    return 0;
#endif

#if 0
// raise 实验
    pid_t pid;

    int ret;

    if ((pid = fork()) < 0)
    {
        printf("fork error \n");
        _exit(0);
    }

    if (pid == 0)
    {
        printf("chile pid is %d, witting \n", getpid());

        raise(SIGSTOP);

        _exit(0);
    }
    else
    {
        sleep(1);

        if ((waitpid(pid, NULL, WNOHANG)) == 0)
        {
            if ((ret = kill(pid, SIGKILL)) == 0)
            {
                printf("parent kill %d\n", pid);
            }
        }
        waitpid(pid, NULL, 0);

        _exit(0);
    }
    return 0;
#endif
    unsigned int seconds;

    printf("this an alarm test function \n");

    // 设置一个六秒后发送的中断信号
    seconds = alarm(20);

    printf("last alarm seconds remaining is %d\n", seconds);

    printf("porcess sleep 5 seconds \n");
    // 休眠5秒
    sleep(5);

    printf("sleep woke up , reset alarm \n");
    // 重新设置一个 5 秒后发出的中断信号
    seconds = alarm(5);
    printf("last alarm seconds, remaining is %d\n", seconds);

    sleep(1);
    printf("last alarm seconds, remaining is %d\n", seconds);
    sleep(1);
    printf("last alarm seconds, remaining is %d\n", seconds);
    sleep(1);
    printf("last alarm seconds, remaining is %d\n", seconds);
 
    printf("endl\n");

    return 0;
}