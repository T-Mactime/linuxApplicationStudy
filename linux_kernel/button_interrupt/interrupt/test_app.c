#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int error = -20;
    int button_status = 0;

    // 打开设备文件
    int fd = open("/dev/button", O_RDWR);
    if (fd < 0) {
        printf("open file error\n");
        return -1;
    }

    printf("wait button\n");

    do {
        // 读取按键状态
        error = read(fd, &button_status, sizeof(button_status));
        if (error < 0) {
            printf("read file error\n");
            return -1;
        }
        usleep(1000 * 100);
    } while (button_status == 0);

    printf("button down\n");

    error = close(fd);
    if (error < 0) {
        printf("close file error\n");
        return -1;
    }

    return 0;
}
