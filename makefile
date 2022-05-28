CC = g++
FLAGS = -Wall -g

all: server client test

client: client.cpp
	$(CC) $(FLAGS) -o client client.cpp	-lpthread

server: server.o stack.o dyalloc.o locker.o
	$(CC) $(FLAGS) -o server server.o stack.o dyalloc.o locker.o -lpthread -lrt

server.o: server.cpp
	$(CC) $(FLAGS) -c server.cpp

stack: stack.o dyalloc.o
	$(CC) $(FLAGS) -o stack stack.o dyalloc.o -lpthread -lrt
stack.o: stack.cpp
	$(CC) $(FLAGS) -c stack.cpp

dyalloc.o: dyalloc.cpp
	$(CC) $(FLAGS) -c dyalloc.cpp

locker.o: locker.cpp
	$(CC) $(FLAGS) -c locker.cpp

test: test.o stack.o dyalloc.o
	$(CC) $(FLAGS) -o test test.o stack.o dyalloc.o

test.o: test.cpp
	$(CC) $(FLAGS) -c test.cpp

clean:
	rm -f *.o client server test locker