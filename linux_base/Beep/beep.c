#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "beep.h"


int beep_init(){

	// 调用内核提供的文件库，文件句柄的返回值是整形
	// 这里并没有c 语言提供的库
	int fd;
		
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if(fd < 0){
		
		return -1;
	}

	// BEEP_GPIO_INDEX 应该是野火定义了一套枚举，这里应该是45
	write(fd, BEEP_GPIO_INDEX, strlen(BEEP_GPIO_INDEX));
	close(fd);

	fd = open("/sys/class/gpio/gpio" BEEP_GPIO_INDEX "/direction", O_WRONLY);
	if(fd < 0){
		return -2;
	}

	write(fd, "out", strlen("out"));

	close(fd);
	return 0;
}

int beep_deinit(){
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if(fd < 0){
		printf("beep unexport open failed\n");
		return -1;
	}

	write(fd, BEEP_GPIO_INDEX, strlen(BEEP_GPIO_INDEX));

	close(fd);

	return 0;
}

int beep_on(void){
	int fd;

	fd = open("/sys/class/gpio/gpio" BEEP_GPIO_INDEX "/value", O_WRONLY);
	if(fd < 0){
		printf("beep value open failed\n");
		return -1;
	}

	write(fd, "1", 1);

	close(fd);
	return 0;
}

int beep_off(void){
	int fd;

	fd = open("/sys/class/gpio/gpio" BEEP_GPIO_INDEX "/value", O_WRONLY);
	if(fd < 0){
		printf("beep value open failed\n");
		return -1;
	}

	write(fd, "0", 1);

	close(fd);
	return 0;
}