#include <stdlib.h>
#include <stdio.h>
#ifdef  __linux__
#include <unistd.h>
#include <signal.h>
#elif __WIN32
#include <windows.h>
#include <iostream>
#endif
void printMessage();

#ifdef  __linux__
void emptyHandle(int signo, siginfo_t* info, void* context){}

void sigTermHandle(int signo, siginfo_t *info, void *context)
{
  lockf(fileno(stdout), F_LOCK, 100);
  printf("Terminated process [ pid = %d ]\n", getpid());
  lockf(fileno(stdout), F_ULOCK, 100);
  sigqueue(getppid(), SIGRTMIN+5, {0});
  exit(0);
}
#endif

int main(int argc, char* argv[])
{
 #ifdef  __linux__
  sigset_t sigSetPrint;
  if (sigemptyset(&sigSetPrint) != 0) {
    puts("sigemtyset error");
    exit(-1);
  }
  if (sigaddset(&sigSetPrint, SIGRTMIN+10) != 0) {
    puts("siaddset error");
    exit(-2);
  }
  struct sigaction actOnPrint;
  actOnPrint.sa_flags = SA_SIGINFO |SA_RESTART;
  actOnPrint.sa_mask = sigSetPrint;
  actOnPrint.sa_sigaction = emptyHandle;
  if (sigaction(SIGRTMIN+10, &actOnPrint, 0) != 0) {
    puts("sigaction error");
    exit(-2);
  }

  sigset_t sigSetTerm;
  if (sigemptyset(&sigSetPrint) != 0) {
    puts("sigemtyset error");
    exit(-1);
  }
  if (sigaddset(&sigSetPrint, SIGTERM) != 0) {
    puts("sigaddset error");
    exit(-2);
  }
  struct sigaction actOnTerm;
  actOnTerm.sa_flags = SA_SIGINFO | SA_RESTART;
  actOnTerm.sa_mask = sigSetTerm;
  actOnTerm.sa_sigaction = sigTermHandle;
  if (sigaction(SIGTERM, &actOnTerm, 0) != 0) {
    puts("sigaction error");
    exit(-2);
  }
  siginfo_t infoPrintSig;
  for(long i = 0; ;i++) {
    sigwaitinfo(&sigSetPrint, &infoPrintSig);
    if(i == 0) {
      printf("Start process [ pid = %d ]\n", getpid());
    } else {
      printMessage();
    }
    usleep(100000);
    sigqueue(getppid(), SIGRTMIN+5, {0});
  }
  return 0;
 #elif _WIN32
  if(argc != 2) {
    printf("argc != 2. Error\n");
    return 1;
  }
  HANDLE hPrintEvent;
  if ((hPrintEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, argv[1])) == 0) {
    printf("OpenEvent() error. last error: ", GetLastError());
    exit(-1);
  }
  HANDLE hManagerEvent;
  if ((hManagerEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, "manager_event")) == 0) {
    printf("OpenEvent() error. last error: ", GetLastError());
    exit(-1);
  }
  HANDLE hTermEvent;
  char termEventName[250];
  strcpy(termEventName, argv[1]);
  strcat(termEventName, "XXX");
  if ((hTermEvent = OpenEventA(EVENT_ALL_ACCESS, true, termEventName)) == 0) {
    printf("OpenEvent() error. last error = ", GetLastError());
    exit(-1);
  }
  for(long i=0;;i++) {
    WaitForSingleObject(hPrintEvent,INFINITE);
    if(WaitForSingleObject(hTermEvent,0) == WAIT_OBJECT_0) {
      if(SetEvent(hManagerEvent) == 0) {
        printf("SetEvent error Manager. last error = %d\n",GetLastError());
        exit(-2);
      }
      CloseHandle(hTermEvent);
      return 0;
    }
    if(i == 0 ) {
      printf("process [ pid = %d ] started ...\n", GetCurrentProcessId());
    }
    printMessage();
    Sleep(100);
    if(SetEvent(hManagerEvent) == 0) {
      printf("SetEvent error. last error = %d\n",GetLastError());
      exit(-2);
    }
  }
    return 0;
 #endif
}

void printMessage()
{
 #ifdef  __linux__
  printf("%s%d%s\n","my pid = ", getpid(), "..........................................................");
 #elif _WIN32
  printf("%s%d%s\n","my pid = ", GetCurrentProcessId(), "..........................................................");
 #endif
}

