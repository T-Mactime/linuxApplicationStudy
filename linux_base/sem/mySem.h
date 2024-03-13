#ifndef _SEM_H_
#define _SEM_H_
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

extern int init_sem(int sem_id, int init_value);
extern int del_sem(int sem_id);
extern int sem_p(int sem_id);
extern int sem_v(int sem_id);

#endif

