#ifndef THREAD_H
#define THREAD_H
#include <system_error>
#include <iostream>
#include <pthread.h>
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <pthread.h>
#endif

class Thread
{
private:
#ifdef _WIN32
  HANDLE mhThreadId;
#elif __linux__
  pthread_t mThreadId;
#endif
public:
  class Exception
  {
   public:
    std::string info;
    Exception(std::string info)
    {
      Exception::info = info;
      std::cerr << info <<". errno = " << std::hex << std::showbase <<errno << std::endl;
    }
  };
#ifdef _WIN32
  int startThread(LPTHREAD_START_ROUTINE startFunc, void* args);
#elif __linux__
  int startThread(void*(*startFunc)(void*), void*args);
#endif
};

#endif // THREAD_H
