CC=gcc
CFLAGS= -Werror=all

.PHONY: all clean

VPATH=.

all: hudini.a

hudini.a: sender.o receiver.o
	ar rcs $@ $^ 

%.o: %.c 
	gcc $(CFLAGS) -c -o $@ $^

clean:
	rm -f *.o
	rm -f *.a