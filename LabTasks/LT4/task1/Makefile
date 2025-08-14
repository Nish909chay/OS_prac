CC = gcc	# tells to use gcc
CFLAGS = -Wall -g 	
# Wall enables all warning
# -g for debugging with gdb

all: program

# link the obj files to the program param
program: main.o a.o b.o
	$(CC) $(CFLAGS) -o program main.o a.o b.o

# compile indiv files to obj files 
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
a.o: a.c
	$(CC) $(CFLAGS) -c a.c
b.o: b.c
	$(CC) $(CFLAGS) -c b.c

cleam:
	rm -f *.o program