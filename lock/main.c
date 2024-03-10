#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUMBER 3

pthread_mutex_t mutex;

void* thread_func(void*arg){
    int num = (unsigned long long) arg;

    // int sleep_time = 0;
    int res;

    res = pthread_mutex_lock(&mutex);
    if(res){
        printf("thread %d lock failed\n", num);

        pthread_mutex_unlock(&mutex);

        pthread_exit(NULL);
    }
    printf("thread %d is hold mutex\n", num);

    sleep(2);

    printf("thread %d freed mutx\n", num);

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(void){
    pthread_t thread[THREAD_NUMBER];
    int num = 0, res;

    srand(time(NULL));

    pthread_mutex_init(&mutex, NULL);

    for(num = 0; num < THREAD_NUMBER; num++){
        res = pthread_create(&thread[num], NULL, thread_func, (void*)(unsigned long long) num);
        if(res != 0){
            printf("crate pthread failed");
            exit(res);
        }
    }

    for(num = 0; num < THREAD_NUMBER; num++){
        pthread_join(thread[num], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}