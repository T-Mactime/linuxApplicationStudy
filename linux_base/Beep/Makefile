ARCH=x86

ifeq ($(ARCH), x86)
	CC=gcc
else ifeq ($(ARCH), arm)
	CC=arm-linux-gnueabihf-gcc-8 
endif

CFLAGS=-g -O0 -Wall

target=beep
source=$(wildcard *.c)
objects=$(source:.c=.o)

$(target): $(objects)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(target) $(objects)
