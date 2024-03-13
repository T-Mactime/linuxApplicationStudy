#include <stdio.h>
#include <string.h> 
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/ioctl.h>

static const char default_path[] = "/dev/ttySTM0";

int main(){
    int res = 0;
    int fd;
    char buf[1024];

    printf("this it tty usart demo \n");

    fd = open(default_path, O_RDWR);
    if(fd < 0) {
        perror(default_path);
        return -1;
    }

    struct termios opt;

    // 清空串口接收缓冲区
    tcflush(fd, TCIOFLUSH);

    tcgetattr(fd, &opt);

    cfsetospeed(&opt, B115200);
    cfsetispeed(&opt, B115200);
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;
    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~INPCK;
    opt.c_cflag &= ~CSTOPB;

    tcsetattr(fd, TCSANOW, &opt);

    printf("device %s is set to 9600 bps 8 N1 \n" , default_path);

    do{
        write(fd, buf, strlen(buf));
        res = read(fd, buf, 1024);
        if(res > 0){
            buf[res] = '\0';
            printf("receive res = %d, btypes data %s\n", res, buf);
        }
    } while (res >= 0);
    
    printf("read error res %d\n", res);

    close(fd);
    return 0;

}
