#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>

// 书上说mp157 只有一个输入事件，但是实际测试发现有两个，event1才是按键输入
const char* path = "/dev/input/event1";

int main(){
    int ret = 0;
    int fd;
    struct input_event event;

    fd = open(path, O_RDONLY);
    if(fd < 0) {
        perror(path);
        return -1;
    }
    while(1){
        ret = read(fd, &event, sizeof(struct input_event));
        if(ret == sizeof(struct input_event)){
            if(event.type != EV_SYN){
                printf("event time %ld %ld \n", event.time.tv_sec, event.time.tv_usec);
                printf("type：%d， code %d, value %d\n", event.type, event.code,
                        event.value);
            }
        }

    }
    close(fd);

	return 0;
}
