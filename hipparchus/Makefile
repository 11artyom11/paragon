CC=gcc
CXX=g++
CFLAGS= -Werror=all
CPPFLAGS= -Werror=all
LIB= -lpthread -lresolv -ldl
.PHONY: all clean

VPATH=.

all: hipparchus.a

hipparchus.a: hipparchus.o
	ar rcs $@ $^ 

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -f *.o
	rm -f *.a