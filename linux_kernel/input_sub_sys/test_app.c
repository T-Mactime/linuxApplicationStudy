#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <linux/input.h>

struct input_event button_inter_event;

int button_status = 0;

int main(int argc, char** argv)
{
    int error = -20;
    int input_status = 0;

    // 打开文件
    int fd = open("/dev/input/event1", O_RDONLY);
    if (fd < 0) {
        printf(" open file error");
        return -1;
    }

    printf("wait button down \n");

    do {
        error = read(fd, &button_inter_event, sizeof(button_inter_event));
        if (error < 0) {
            printf("read failed eror \n");
        }

        if (button_inter_event.type == 1 && button_inter_event.code == 2) {
            if (button_inter_event.value == 0) {
                printf("button up\n");
            } else if (button_inter_event.value == 1) {
                printf("button down\n");
            }
        }
    } while (1);

    printf(" button down \n");

    error = close (fd);
    if(error < 0)
    {
        printf("close file error \n");
    }
    return 0;
}