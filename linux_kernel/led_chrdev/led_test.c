#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

char *led_on = "0\n";
char *led_off = "1\n";

void onLed(int fd)
{
    printf("on led\n");
    write(fd, led_on, strlen(led_on));
}

void offLed(int fd)
{
    printf("off led\n");
    write(fd, led_off, strlen(led_on));
}

int main(void)
{
    // printf("led_cdev test\n");
    // //打开设备
    // int fd = open("/dev/LedChrDEV0", O_RDWR);
    // if(fd>0)
    //     printf("led_chrdev0 open success\n");
    // else
    //     printf("led_chrdev0 open fail\n");
    // //写入数据
    // write(fd, led_on, strlen(led_on));
    // //写入完毕，关闭文件
    // close(fd);
    // sleep(1);
    // //打开设备
    //  fd = open("/dev/LedChrDEV1", O_RDWR);
    // if(fd>0)
    //     printf("led_chrdev1 open success\n");
    // else
    //     printf("led_chrdev1 open fail\n");

    // //写入数据
    // write(fd, led_on, strlen(led_on));
    // //写入完毕，关闭文件
    // close(fd);
    // sleep(1);
    //  //打开设备
    //  fd = open("/dev/LedChrDEV2", O_RDWR);
    // if(fd>0)
    //     printf("led_chrdev2 open success\n");
    // else
    //     printf("led_chrdev2 open fail\n");
    // //写入数据
    // write(fd, led_on, strlen(led_on));
    // //写入完毕，关闭文件
    // close(fd);
    // sleep(1);

    // //关闭设备
    // fd = open("/dev/LedChrDEV0", O_RDWR);
    // write(fd, led_off, strlen(led_off));
    // close(fd);
    //  fd = open("/dev/LedChrDEV1", O_RDWR);
    // write(fd, led_off, strlen(led_off));
    // close(fd);
    //  fd = open("/dev/LedChrDEV2", O_RDWR);
    // write(fd, led_off, strlen(led_off));
    // close(fd);

    int fd1, fd2;

    fd1 = open("/dev/LedChrDEV0", O_RDWR);
    if (fd1 < 0)
    {
        printf("open led1 file failed\n");
        return -1;
    }

    fd2 = open("/dev/LedChrDEV2", O_RDWR);
    if (fd2 < 0)
    {
        printf("open led1 file failed\n");
        return -1;
    }

    int i, j, k;
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (j == 0)
            {
                onLed(fd1);
                usleep(200000);
                offLed(fd1);
                sleep(1);
            }
            else
            {
                onLed(fd1);
                usleep(200000);
                offLed(fd1);
                usleep(300000);
            }
        }
        sleep(2);
        for (k = 0; k < 4; k++)
        {
            onLed(fd2);
            if (k == 0)
            {
                onLed(fd2);
                usleep(200000);
                offLed(fd2);
                sleep(1);
            }
            else
            {
                onLed(fd2);
                usleep(200000);
                offLed(fd2);
                usleep(300000);
            }
        }
    }

    offLed(fd1);
    offLed(fd2);

    return 0;
}