ARCH=x86

ifeq ($(ARCH),x86)
	CC=gcc
else ifeq ($(ARCH),arm)
	CC=arm-linux-gnueabihf-gcc-8 
endif

CFLAGS = -O0 -g -Wall

sources = $(wildcard *.c)
objs = $(sources:.c=.o)

target=input

$(target):$(objs)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(target) $(objs)
