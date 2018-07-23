
CC= gcc
CFLAGS= -Wall -g 
CLIBS= -lcunit
GCOVFLAGS= -Wall 

all: main

main: main.o malloc.o
	$(CC) $(GCOVFLAGS) -o $@ $< $(CLIBS)

clean:
	rm *.o main

.SUFFIXES: .h .c .o

%.o: %.c
	$(CC) $(GCOVFLAGS) -c -o $@ $< 

i:
	indent *.c *.h

