ARCH=x86

ifeq ($(ARCH),x86)
	CC=gcc
else ifeq ($(ARCH),arm)
	CC=arm-linux-gnueabihf-gcc-8
endif


sources=$(wildcard *.c)
objs=$(sources:.c=.o)

target=forkTest

CFLAGS += -O0 -g -Wall

$(target):$(objs)
	$(CC) $(CFLAGS) -o $@ $^ 

.PHONY: clean

clean:
	rm -f $(target) $(objs) 