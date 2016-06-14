#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <iostream>

#ifdef __linux__
  #include <sys/types.h>
  #include <sys/ipc.h>
  #include <sys/sem.h>
  #include <err.h>
#elif _WIN32
  #include <windows.h>
#endif //_WIN32

class Semaphore
{
 private:
#ifdef __linux__
  int mSemId;
#elif _WIN32
  HANDLE mHsem;
#endif
 public:
  class Exception
  {
   public:
    std::string info;
    Exception(std::string info)
    {
      Exception::info = info;
      std::cerr << info.c_str() << ". errno = " << std::hex << std::showbase <<errno << std::endl;
    }
  };

  Semaphore();
  void create(int key);
  void attach(int key);
  void remove();
  /*if mSec == -1. wait Infinite*/
  void waitNotZero(int mSec = -1);
  void op(int value);
};

#endif // SEMAPHORE_H
