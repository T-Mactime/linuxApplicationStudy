
ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

export ARCH CROSS_COMPILE

obj-m := led_chrdev.o

out = led_test

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o $(out) led_test.c

.PHONY: clean copy

clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm -f $(out)
