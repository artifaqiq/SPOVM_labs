#ifdef  __linux__
  #include <signal.h>
  #include <sys/wait.h>
  #include <unistd.h>
  #include <err.h>
  #include <errno.h>
#endif

#include <iostream>
#include "processinfo.h"

ProcessInfo::ProcessInfo() { }

void ProcessInfo::terminate()
{
 #ifdef  __linux__
  if(mPid == 0) {
    return;
  }
  kill(SIGKILL, mPid);
  waitpid(mPid, 0, WNOHANG);
  mPid=0;
 #elif _WIN32
  if(TerminateProcess(mPinf.hProcess,0) == false) {
    std::cerr << "Exception. TerminateProcess error. last error: " << GetLastError() << std::endl;
    throw ProcessInfo::Exception("TerminateProcess");
    }
  CloseHandle(mPinf.hProcess);
  CloseHandle(mPinf.hThread);
 #endif
}

int ProcessInfo::getCurrentPid() {
 #ifdef  __linux__
  return getpid();
 #elif _WIN32
  return GetCurrentProcessId();
 #endif
}

int ProcessInfo::getPid()
{
 #ifdef  __linux__
  return mPid;
 #elif _WIN32
  return mPinf.dwProcessId;
 #endif
}

void ProcessInfo::createAndExec(std::string filename)
{
 #ifdef  __linux__
  pid_t pid;
  pid = fork();
  if(pid < 0) {
    std::cerr << "Exception. Fork() error" << std::endl;
    throw Exception("fork error");
    } else if(pid == 0) {
      execl(filename.c_str(), 0);
    } else {
      mPid=pid;
    }
 #elif _WIN32
  STARTUPINFOA stInfo;
  ZeroMemory(&stInfo, sizeof(stInfo));
  stInfo.cb = sizeof(stInfo);
  ZeroMemory(&mPinf, sizeof(mPinf));
  if(CreateProcessA(nullptr,
                    const_cast<LPSTR>(filename.c_str()),
                    NULL,
                    NULL,
                    false,
                    NULL,
                    NULL,
                    NULL,
                    &stInfo,
                    &mPinf) == false) {
    std::cerr << "Exception. CreateProcess() error. last error: " << GetLastError() << std::endl;
    throw ProcessInfo::Exception ("CreateProcess() error");
    }
 #endif
}

#ifdef  __linux__

void ProcessInfo::createAndFunc(void (*handle)())
{
  pid_t pid;
  pid = fork();
  if(pid < 0) {
    throw Exception("fork error");
  } else if(pid == 0) {
    handle();
    exit(0);
  } else {
    if(waitpid(pid, nullptr, 0) != 0) {
      std::cerr << "waitpid error. errno1 = " << errno << std::endl;
      throw ProcessInfo::Exception("waitpid error");
    }
    mPid=pid;
  }
}

void ProcessInfo::sendSignal(int signo) { kill(mPid, signo); }

#endif

ProcessInfo::Exception::Exception(std::string info) { Exception::info = info; }

std::string ProcessInfo::Exception::getInfo() { return Exception::info; }
