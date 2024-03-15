#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

char*wbuf = "hello world\n";

char buf[128];


int main(){
    printf("embed cahr dev test\n");

    int fd = open("/dev/chrdev", O_RDWR);
    write(fd, wbuf, strlen(wbuf));

    close(fd);

    fd = open("/dev/chrdev", O_RDWR);

    read(fd, buf, 128);

    printf("the con %s\n", buf);

    close(fd);
    return 0;

}