CFLAGS=-D__LINUX__ -c -Wall -O2 
LIBS = 

all: demo 

demo: main.o
	$(CC) main.o $(LIBS) -o demo 

main.o: main.c
	$(CC) $(CFLAGS) main.c

clean:
	rm -rf *.o demo
