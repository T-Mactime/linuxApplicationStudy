ARCH=x86

ifeq ($(ARCH),x86)
    CC=gcc
else ifeq ($(ARCH),arm)
    CC=arm-linux-gnueabihf-gcc-8
endif

recvSource=recv.c
sendSource=send.c
recvObjs=$(recvSource:.c=.o)
sendObjs=$(sendSource:.c=.o)

msgRecv=recv
msgSend=send

CFLAGS += -g -Wall -O0

.PHONY: clean recv send

all: recv send

recv: $(msgRecv)

$(msgRecv): $(recvObjs)
	$(CC) $(CFLAGS) -o $@ $^

send: $(msgSend)

$(msgSend): $(sendObjs)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(msgRecv) $(msgSend) $(recvObjs) $(sendObjs)
