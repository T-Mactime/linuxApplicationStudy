KERNEL_DIR=/home/sch/Documents/ebf_linux_kernel_mp157_depth1/build_image/build
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
export  ARCH  CROSS_COMPILE

obj-m := hellomodule.o
all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules

.PHONE:clean copy

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean	

copy:
	sudo  cp  *.ko  /home/embedfire/workdir
