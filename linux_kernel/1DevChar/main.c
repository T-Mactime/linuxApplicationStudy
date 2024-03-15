#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

char *wbuf = "helloworld \n";
char rbuf[128];



int main(){
    printf("embde char dev tst \n");

    int fd = open("/dev/chrdev", O_RDWR);
    if(fd < 0){
        printf("get file error\n");
        return -1;
    }

    write(fd, wbuf, strlen(wbuf));

    close (fd);

    fd = open("/dev/chrdev", O_RDWR);

    read(fd, rbuf, 128);

    printf("end of read %s\n", rbuf);

    close(fd);

    return 0;
}