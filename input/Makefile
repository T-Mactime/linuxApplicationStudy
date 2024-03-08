CC=gcc

CFLAGS = -O0 -g -Wall

sources = $(wildcard *.c)
objs = $(sources: .c=.o)

target=input

$(target):$(objs)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm $(target) $(objs) -f
