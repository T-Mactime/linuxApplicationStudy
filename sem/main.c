#include "mySem.h"

#define DELA_TIME 3

int main()
{
    pid_t result;

    int sem_id;

    sem_id = semget((key_t)6666, 1, 0666 | IPC_CREAT);

    init_sem(sem_id, 0);

    result = fork();

    if (result == -1)
    {
        perror("Fork \n");
    }
    else if (result == 0)
    {
        printf("clild process will wait for some seconds \n");
        sleep(DELA_TIME);
        printf("the return  %d, pid %d\n", result, getpid());
        sem_v(sem_id);
    }
    else
    {
        sem_p(sem_id);

        printf("then value %d, pid %d", result, getpid());

        sem_v(sem_id);
        del_sem(sem_id);
    }
    exit(0);
}