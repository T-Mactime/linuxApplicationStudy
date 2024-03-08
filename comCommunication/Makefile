CC=gcc
CFLAGS=-O0 -g -Wall

sources+=main.c
objs=$(sources:.c=.o)
target=com

iosources+=iomain.c
ioobjs=$(iosources:.c=.o)
ioctl=ico

$(target): $(sources)
	$(CC) $(CFLAGS) -o $@ $^

$(ioctl): $(ioobjs)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: io clean

io: $(ioctl)

clean:
	rm $(objs) $(target) $(ioctl) -f

