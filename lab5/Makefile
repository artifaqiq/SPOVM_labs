CC = gcc
CC_FLAGS = -lrt -g -Wall -pedantic
LIBS_LIN = -lrt -lpthread -ldl
all:
ifeq ($(OS), Windows_NT)
$(CC) -o lab5 main.c fqueue_win.c thread_win.c $(CC_FLAGS)
else
$(CC) -o lab5 main.c fqueue_lin.c thread_lin.c $(LIBS_LIN) $(CC_FLAGS)
endif

libs:
ifeq ($(OS), Windows_NT)
gcc -c async_win.c $(CC_FLAGS)
gcc -shared -o async_win.dll async_win.o $(CC_FLAGS)
del /Q async_win.o
else
gcc -c -fPIC async_lin.c $(CC_FLAGS)
gcc -shared -o async_lin.so async_lin.o $(CC_FLAGS)
rm async_lin.o
endif

clean:
ifeq ($(OS), Windows_NT)
del /Q lab5.exe
del /Q async_win.dll
else
rm -f lab5
rm -f async_lin.so
endif

remake:
clean libs all
