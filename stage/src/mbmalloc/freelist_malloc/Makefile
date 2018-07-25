
CC= gcc
VERBOSE=-DVERBOSE_M
CFLAGS= -Wall -g $(VERBOSE)
CLIBS= -lcunit
GCOVFLAGS= -Wall $(VERBOSE) 

all: main

main: main.o malloc.o
	$(CC) $(GCOVFLAGS) -o $@ $< $(CLIBS)

clean:
	rm *.o main *~

.SUFFIXES: .h .c .o

%.o: %.c
	$(CC) $(GCOVFLAGS) -c -o $@ $< 

i:
	indent *.c *.h

