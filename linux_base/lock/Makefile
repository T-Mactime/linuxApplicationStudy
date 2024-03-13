ARCH=x86

ifeq ($(ARCH),x86)
	CC=gcc
else ifeq ($(ARCH),arm)
	CC=arm-linux-gnueabihf-gcc-8
endif

CFLAGS+=-O0 -g -Wall
target=lock 

sources = $(wildcard *.c)
objs = $(sources:.c=.o)

LDFLAGS+=-lpthread

$(target):$(objs)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY:clean

clean:
	rm -f $(target) $(objs)