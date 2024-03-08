CC=gcc
CFLAGS=-g -O0 -Wall

target=beep
source=$(wildcard *.c)
objects=$(source:.c=.o)

$(target): $(objects)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(target) $(objects)
