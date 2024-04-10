ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

export ARCH CROSS_COMPILE

obj-m := i2c_pinctrl.o
out = i2c_app

all:
	$(MAKE) EXTRA_CFLAGS=-fno-pic -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o $(out) i2c_app.c

.PHONY: clean 

clean: 
	$(MAKE) -C $(KERNEL_DIR) -M $(CURDIR) clean
	rm -f $(out)