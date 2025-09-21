CC = gcc	# tells to use gcc
CFLAGS = -Wall -g 	
	# Wall enables all warning
# -g for debugging with gdb

all: program # all simply sets the target

# link the obj files to the program param
program: main.o a.o b.o    # to build or rebuild 
	$(CC) $(CFLAGS) -o program main.o a.o b.o

# compile indiv files to obj files 
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
a.o: a.c
	$(CC) $(CFLAGS) -c a.c
b.o: b.c
	$(CC) $(CFLAGS) -c b.c

clean:
	rm -f *.o program

# $(CC) will expand to gcc or whatever compiler value used 
# $(CFLAGS) will also expland to the compiler flags , in our case we used -Wall meaning all warning 	
# -f means ignore errors 	
