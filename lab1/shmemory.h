#ifndef SHMEMORY_H
#define SHMEMORY_H

#include <iostream>
#include <string>
#include <errno.h>

#ifdef __linux
 #include <unistd.h>
 #include <sys/ipc.h>
 #include <sys/shm.h>
 #include <err.h>
#elif _WIN32
 #include <Windows.h>
#endif

class ShMemory
{
 private:
  static const int SHM_DEFAULT_SIZE = 4096;
  void* mShmAddr;
#ifdef __linux__
  int mShmId;   
#elif _WIN32
 HANDLE mhMap;   
#endif
 public:
  class Exception
  {
   public:
    std::string info;
    Exception(std::string info)
    {
      Exception::info = info;
      std::cerr << info <<". errno = " << errno << std::endl;
    }
  };

  ShMemory();
  void* create(int key, int size = SHM_DEFAULT_SIZE);
  void* attach(int key);
  void detach();
  void remove();
};

#endif // SHMEMORY_H
