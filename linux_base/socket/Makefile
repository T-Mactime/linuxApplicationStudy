

ARCH=x86

ifeq ($(ARCH),x86)
	CC=gcc
else ifeq ($(ARCH),arm)
	CC=arm-linux-gnueabihf-gcc-8
endif

CFLAGS+=-O0 -g -Wall

reSources = recv.c
sdSources = send.c

reObjs = $(reSources:.c=.o)
sdObjs = $(sdSources:.c=.o)

reTar = re
sdTar = sd
.PHONY: recv send clean

all: recv send

recv:$(reTar)

$(reTar):$(reObjs)
	$(CC) $(CFLAGS) -o $@ $^

send:$(sdTar)

$(sdTar):$(sdObjs)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(sdTar) $(reTar) $(sdObjs) $(reObjs)