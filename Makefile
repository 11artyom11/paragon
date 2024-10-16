CC=gcc
HUDINI_DIR = ./hudini
HIPPARCHUS_DIR = ./hipparchus
REMBRANDT_DIR = ./rembrandt
INCLUDE_PATH= -I$(HUDINI_DIR) -I$(REMBRANDT_DIR) -I$(HIPPARCHUS_DIR)
CFLAGS=  $(INCLUDE_PATH)
CPPFLAGS= $(INCLUDE_PATH) 
PARAGON=paragon

.PHONY: all clean

VPATH=.

all: paragon hudini.a

paragon:  main.o hudini.a hipparchus.a rembrandt.a 
	g++ -o $(PARAGON)  main.o $(HUDINI_DIR)/hudini.a $(HIPPARCHUS_DIR)/hipparchus.a  $(REMBRANDT_DIR)/rembrandt.a  $(HIPPARCHUS_DIR)/lib/libchilkat.a -lglut -lGLU -lGL -lm -ldl -lpthread

hudini.a:
	cd $(HUDINI_DIR) && $(MAKE) all

hipparchus.a:
	cd $(HIPPARCHUS_DIR) && $(MAKE) all

rembrandt.a:
	cd $(REMBRANDT_DIR) && $(MAKE) all

%.o: %.cpp 
	g++ $(CPPFLAGS) $() -c -o $@ $^

%.o: %.c
	gcc $(CFLAGS) $() -c -o $@ $^

clean:
	rm -f *.o
	rm -f $(PARAGON)
	cd $(HUDINI_DIR) && $(MAKE) clean
	cd $(HIPPARCHUS_DIR) && $(MAKE) clean
	cd $(REMBRANDT_DIR) && $(MAKE) clean
