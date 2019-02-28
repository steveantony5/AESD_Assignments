CC = gcc
Flags = -Wall -g -lpthread -lrt -lm

all : thread.c POSIX_Timer.c

	$(CC) thread.c POSIX_Timer.c -o thread $(Flags)

clean: 
	rm -f thread POSIX_Timer