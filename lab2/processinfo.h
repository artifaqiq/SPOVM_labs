#ifndef PROCESSINFO_H
#define PROCESSINFO_H
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

class ProcessInfo
{
 private:
 #ifdef  __linux__
  pid_t mPid = 0;
 #elif _WIN32
  PROCESS_INFORMATION mPinf;
 #endif
 public:
  class Exception
  {
   protected:
    std::string info;
   public:
    Exception(std::string info);
    std::string getInfo();
  };
  ProcessInfo();
  void terminate();
  int getPid();
  void createAndExec(std::string filename);
  static int getCurrentPid();
 #ifdef  __linux__
  void sendSignal(int signo);
  void createAndFunc(void (*handle)(void));
 #endif
};

#endif // PROCESSINFO_H
