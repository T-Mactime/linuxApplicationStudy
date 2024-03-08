#include <stdio.h>
#include <unistd.h>
#include "beep.h"


int main(){

	int ret = 0;
	beep_init();
	if(ret != 0){
		printf("beep init failed\n");
		return -1;
	}

	ret = beep_on();
	if(ret != 0){
		printf("beep on failed\n");
		return -1;
	}


	sleep(1);

	beep_off();
	if(ret != 0){
		printf("beep off failed\n");
		return -1;
	}	
	beep_deinit();
	if(ret != 0){
		printf("beep deinit failed\n");
		return -1;
	}

	return 0;
}
