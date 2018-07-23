
CC=gcc -g
CCOBJ=leslie.o user_leslie.o

all: user

user: $(CCOBJ)
	$(CC) -o user_leslie $(CCOBJ) 

.c.o: %.c
	$(CC) -c -o $*.o $< 

clean:
	rm *.o user_leslie
