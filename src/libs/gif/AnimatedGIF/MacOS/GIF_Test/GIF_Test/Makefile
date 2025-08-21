CFLAGS=-Wall -O2 
LIBS = 

all: giftest

giftest: main.o
	$(CC) main.o $(LIBS) -o giftest 

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

clean:
	rm -rf *.o giftest
