#ifdef __linux__
#include <unistd.h>
#include <signal.h>
#elif _WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <iostream>
#include <stdio.h>
#include <vector>
#include "processmanager.h"

ProcessManager manager;
#ifdef __linux__
#define getSymbol getchar

void handlerEmpty(int signo, siginfo_t* info, void* context) { }

void handlerSigIsPrint(int signo, siginfo_t* info, void* context) { manager.startManager(); }

#elif _WIN32
#define getSymbol getchar_noblock
int getchar_noblock()
{
    if (_kbhit())
        return _getch();
    else
        return -1;
}
#endif

int main()
{
  std::cout << "Main pid = " << ProcessInfo::getCurrentPid() << std::endl;
  std::cout << "+ add process\n- kill last process\nq exit program\n";
 #ifdef __linux__;
  sigset_t setSigWait;
  if(sigemptyset(&setSigWait) != 0) {
    std::cerr << "sigemptyset error. errno = " << errno << std::endl;
    exit(-1);
  }
  if(sigaddset(&setSigWait, SIGRTMIN + 5) != 0) {
    std::cerr << "sigemptyset error. errno = " << errno << std::endl;
    exit(-2);
  }
  struct sigaction actOnSig;
  actOnSig.sa_mask = setSigWait;
  actOnSig.sa_sigaction = handlerSigIsPrint;
  actOnSig.sa_flags = SA_RESTART | SA_SIGINFO;
  if(sigaction(SIGRTMIN + 5, &actOnSig, nullptr) != 0) {
    std::cerr << "sigaction error. errno = " << errno << std::endl;
    exit(errno);
  }
 #elif _WIN32
  SECURITY_ATTRIBUTES secAttr;
  secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  secAttr.bInheritHandle = true;
  secAttr.lpSecurityDescriptor = 0;
  HANDLE hEvent;
  if((hEvent=CreateEventA(&secAttr, false, false,"manager_event")) == 0) {
    std::cerr << "Error. last error = " << GetLastError() << std::endl;
    exit(-1);
  }
  if(GetLastError() == ERROR_ALREADY_EXISTS) {
    std::cerr << "Error. last error = ERROR_ALREADY_EXISTS" << std::endl;
    exit(-2);
  }
  if(ResetEvent(hEvent) == false) {
    std::cerr << "Error ResetEvent(). last error = " << GetLastError() << std::endl;
    exit(-3);
  }
 #endif
  char inputSymbol;
  while((inputSymbol = getSymbol()) != 'q') {
  switch(inputSymbol) {
    case '+':
      try {
        manager.addProcess("printpidprog");
        if(manager.countProc() == 1) {
          manager.startManager();
        }
      } catch (ProcessManager::Exception) {
        manager.termAll();
       #ifdef _WIN32
        CloseHandle(hEvent);
       #endif
        exit(-3);
      }
      break;
    case '-':
      try {
        int lastTerm;
        lastTerm = manager.termLast();
        if(lastTerm == 0) {
          std::cout << "First add process"<< std::endl;
          break;
        }
      } catch (ProcessManager::Exception) {
       #ifdef _WIN32
        CloseHandle(hEvent);
       #endif
        manager.termAll();
        exit(-4);
       }
    default:
     #ifdef _WIN32
      if (manager.countProc() > 1) {
        WaitForSingleObject(hEvent, INFINITE);
        manager.startManager();
      } else if(manager.countProc() == 1){
        manager.startManager();
      }
     #endif
      break;
    }
  }
  manager.termAll();
 #ifdef _WIN32
  CloseHandle(hEvent);
 #endif
  return 0;
}