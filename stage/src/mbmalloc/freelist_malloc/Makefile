
CC=gcc -g
CCOBJ= main.o malloc.o

all: user

user: $(CCOBJ)
	$(CC) -o main $(CCOBJ) 

.c.o: %.c
	$(CC) -c -o $*.o $< 

clean:
	rm *.o main
i:
	indent *.c *.h
