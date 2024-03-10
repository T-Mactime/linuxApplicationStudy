

#include "mySem.h"

int init_sem(int sem_id, int init_value){
    union semun sem_union;
    sem_union.val = init_value;

    if(semctl(sem_id, 0, SETVAL, sem_union) == -1){
        perror("initialize semaphore");
        return -1;
    }    

    return 0;
}

int del_sem(int sem_id){
    union semun sem_union;
    if(semctl(sem_id, 0, IPC_CREAT, sem_union) == -1){
        perror("delete semaphore");
        return -1;
    }
    return 0;
}


int sem_p(int sem_id){
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = SEM_UNDO;

    if(semop(sem_id, &sops, 1) == -1){
        perror("p operation");
        return -1;
    }
    return 0;
}

int sem_v(int sem_id){
    struct sembuf sops;
    sops.sem_flg = SEM_UNDO;
    sops.sem_num = 0;
    sops.sem_op = 1;

    if(semop(sem_id, &sops, 1) == -1){
        perror("v operation");
        return -1;
    }

    return 0;
}



