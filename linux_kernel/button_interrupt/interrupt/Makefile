ARCH=arm 
CROSS_COMPILE=arm-linux-gnueabihf-

export ARCH CROSS_COMPILE

obj-m := interrupt.o
out = test_app

all:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(MAKE)-gcc -o $(out) test_app.c

.PHONY : clean

clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean
	rm -f $(out)
