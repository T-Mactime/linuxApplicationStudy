ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

export ARCH CROSS_COMPILE

obj-m := rgb_leds.o
out = rgb_leds_app

all: 
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) modules
	$(CROSS_COMPILE)gcc -o $(out) rgb_leds_app.c

.PHONY: clean

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) clean

