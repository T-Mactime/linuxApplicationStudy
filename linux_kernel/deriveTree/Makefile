ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-

export ARCH CROSS_COMPILE

obj-m := get_dts_info.o
all: 
	$(MAKE) -C $(KERNER_DIR) M=$(CURDIR) modules

.PHONY : clean

clean:
	$(MAKE) -C $(KERNER_DIR) M=$(CURDIR) clean