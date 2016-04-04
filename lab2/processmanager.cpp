#include "processmanager.h"
#include <iostream>

#ifdef  __linux__
  #include <signal.h>
  #include <unistd.h>
  #include <sys/wait.h>
  #include <err.h>
#elif _WIN32
  #include <cmath>
#endif

ProcessManager::ProcessManager() { }

void ProcessManager::addProcess(std::string pathname)
{
 #ifdef  __linux__
  try {
    ProcessInfo tempProc;
    tempProc.createAndExec(pathname);
    mListProc.push_back(tempProc);
    mPriorityProcess = mListProc.begin();
  } catch(ProcessInfo::Exception exc) {
    throw ProcessManager::Exception(exc.getInfo());
  }
 #elif _WIN32
  SECURITY_ATTRIBUTES secAttr;
  secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  secAttr.bInheritHandle = true;
  secAttr.lpSecurityDescriptor = 0;
  
  char eventName[20];
  sprintf(eventName, "%d", this->countProc()+1);
  char cmdLine[250];
  strcpy (cmdLine, pathname.c_str());
  strcat(cmdLine, " ");
  strcat(cmdLine, eventName);
  
  HANDLE hEventPrint;
  if((hEventPrint = CreateEventA(&secAttr, false, false, eventName)) == 0) {
    std::cerr << "Exc. last error = " << GetLastError() << std::endl;
    throw ProcessManager::Exception ("addProcess exception");
  }
  mEventsPrint.push_back(hEventPrint);
  mPriorityProcess = mEventsPrint.end();
  mPriorityProcess--;

  strcat(eventName, "XXX");
  HANDLE hEventTerm;
  if((hEventTerm = CreateEventA(&secAttr, false, false, eventName)) == 0) {
    std::cerr << "Exc2. last error = " << GetLastError() << std::endl;
    throw ProcessManager::Exception ("addProcess exception");
  }
  mEventsTerm.push_back(hEventTerm);

  ProcessInfo tempProc;
  try {
    tempProc.createAndExec(cmdLine);
  } catch (ProcessInfo::Exception exc) {
    throw ProcessManager::Exception(exc.getInfo());
  }
  mListProc.push_back(tempProc);
 #endif
}

void ProcessManager::termAll()
{
 #ifdef _WIN32
  auto itEventPrint = mEventsPrint.begin();
  auto itEventTerm = mEventsTerm.begin();
 #endif
  auto itProc = mListProc.begin();
  for(;itProc != mListProc.end();itProc++) {
    try {
      itProc->terminate();
     #ifdef _WIN32
      CloseHandle(*itEventPrint);
      CloseHandle(*itEventTerm);
      itEventPrint++;
      itEventTerm++;
     #endif
    } catch (ProcessInfo::Exception exc) {
      throw ProcessManager::Exception (exc.getInfo());
    }
  }
}

int ProcessManager::countProc()
{
  return mListProc.size();
}

int ProcessManager::termLast()
{
  if(mListProc.empty()) {
    return 0;
  }
  ProcessInfo termProc = mListProc.back();
  try {
   #ifdef _WIN32
    if(mPriorityProcess + 1 == mEventsPrint.end()) {
      mPriorityProcess = mEventsPrint.begin();
    }
    SetEvent(mEventsTerm.back());
    SetEvent(mEventsPrint.back());
    CloseHandle(mEventsPrint.back());
    mEventsPrint.pop_back();
    mEventsTerm.pop_back();
    mListProc.pop_back();
   #elif  __linux__
    if(mPriorityProcess + 1 == mListProc.end()) {
      mPriorityProcess = mListProc.begin();
    }
    mListProc.back().terminate();
    int exitCode;
    waitpid(termProc.getPid(), &exitCode, WNOHANG);
    mListProc.pop_back();
   #endif
    return termProc.getPid();
   } catch (ProcessInfo::Exception exc) {
    throw (exc.getInfo());
   }
}

void ProcessManager::startManager()
{
  if(this->isEmpty()){
    return;
  }
 #ifdef  __linux__
  usleep(20000);
  if(sigqueue(mPriorityProcess->getPid(), SIGRTMIN+10, {0}) < 0) {
    std::cerr << "sigqueue error. errno = " << errno << std::endl;
    throw ProcessManager::Exception("sigqueue error");
  }
  if(++mPriorityProcess == mListProc.end()) {
  mPriorityProcess = mListProc.begin();
  }
 #elif _WIN32
  Sleep(20);
  if(SetEvent(*mPriorityProcess) == 0) {
    std::cerr << "SetEvent error. last error = " << GetLastError() << std::endl;
  }
  if(++mPriorityProcess == mEventsPrint.end()) {
    mPriorityProcess = mEventsPrint.begin();
  }
 #endif
}

bool ProcessManager::isEmpty() { return mListProc.empty(); }

ProcessManager::Exception::Exception(std::string info) { Exception::info=info; }

std::string ProcessManager::Exception::getInfo() { return Exception::info; }
