CC=g++
CFLAGS=-c -Wall -std=c++11

all: linuxProg

linuxProg: mainLinux.o shmemory.o 
	$(CC) mainLinux.o shmemory.o -o linuxProg

mainLinux.o: mainLinux.cpp
	$(CC) $(CFLAGS) mainLinux.cpp

shmemory.o: shmemory.cpp
	$(CC) $(CFLAGS) shmemory.cpp

clean:
	rm -rf *.o linuxProg