#include "thread.h"
#include <iostream>

#ifdef _WIN32
int Thread::startThread(LPTHREAD_START_ROUTINE startFunc, void* args)
{
  mhThreadId = CreateThread(NULL,
                            STACK_SIZE_PARAM_IS_A_RESERVATION,
                            startFunc,
                            args,
                            0,
                            NULL);
  if(mhThreadId == NULL) {
    throw Thread::Exception("CreateThread() == NULL");
  }
  return 0;
}
#elif __linux__
int Thread::startThread(void*(*startFunc)(void*), void* args)
{
  if(0 != pthread_create(&mThreadId, NULL, startFunc, (void*)args)) {
    throw Thread::Exception("pthread_create == NULL");
  }
  return 0;
}

#endif
