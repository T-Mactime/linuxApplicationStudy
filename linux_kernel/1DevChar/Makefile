KERNER_DIR=/home/sch/Documents/ebf_linux_kernel/build_image/build

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH CROSS_COMPILE

obj-m := chrdev.o
out = 1suport

all:
	$(MAKE) -C $(KERNER_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o $(out) main.c

.PHONY: clean

clean :
	$(MAKE) -C $(KERNER_DIR) M=$(CURDIR) clean 
	rm -f $(out)