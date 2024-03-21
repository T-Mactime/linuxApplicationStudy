#KERNEL_DIR=$(ARM_KERNEL_DIR)

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
export  ARCH  CROSS_COMPILE

obj-m := led_device.o led_driver.o


all:
	$(MAKE) -C $(KERNER_DIR) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(KERNER_DIR) M=$(CURDIR) clean

