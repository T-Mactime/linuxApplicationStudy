#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char**argv){
    printf("led tiny test \n");

    if(argc != 2){
        printf("command error \n");
        printf("user\n");
        return -1;
    }

    int fd = open("/dev/rgb_led", O_RDWR);
    if(fd < 0){
        printf("open file \n");
        return -1;
    }

    unsigned char command = atoi(argv[1]);

    /* 写入命令 */
    int error = write(fd, &command, sizeof(command));
    if(error < 0){
        printf("write fd error\n");
        close(fd);
    }

    error = close(fd);
    if(error < 0){
        printf("close file\n");
    }

    return 0;
}