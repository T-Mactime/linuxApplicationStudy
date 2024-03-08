#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <string.h>

static const char default_path[] = "/dev/ttySTM0";

int main(int argc, char** argv){
    int fd;

    struct termios opt;

    char buf[1024] = "Embde fire tty send test\n";

    fd = open(default_path, O_RDWR);
    if(fd < 0) {
        perror(default_path);
        return -1;
    }

    ioctl(fd, TCGETS, &opt);

    opt.c_ispeed = opt.c_cflag & (CBAUD | CBAUDEX);
    opt.c_ospeed = opt.c_cflag & (CBAUD | CBAUDEX);

    speed_t change_speed = B115200;

    cfsetospeed(&opt, change_speed);
    cfsetospeed(&opt, change_speed);

    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;
    
    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~INPCK;
    opt.c_cflag &= ~CSTOPB;

    ioctl(fd, TCSETS, &opt);

    do{
        write(fd, buf, strlen(buf));
    }while (0);

    close (fd);

    return 0;
}
    
