
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH CROSS_COMPILE

obj-m := para.o cala.o

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules

.PHONY: clean copy

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean

copy:
	sudo cp *.ko /home/embedfire/workdir
