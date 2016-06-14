#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <vector>
#include "processinfo.h"

class ProcessManager
{
 private:
  std::vector<ProcessInfo> mListProc;
 #ifdef  __linux__
  std::vector<ProcessInfo>::iterator mPriorityProcess;
 #elif _WIN32
  std::vector<HANDLE> mEventsPrint;
  std::vector<HANDLE> mEventsTerm;
  std::vector<HANDLE>::iterator mPriorityProcess;
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
  ProcessManager();
  void startManager();
  void addProcess(std::string pathname);
  int termLast();
  void termAll();
  bool isEmpty();
  int countProc();
};

#endif // PROCESSMANAGER_H
