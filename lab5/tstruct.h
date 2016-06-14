#ifndef TSTRUCT_H_
#define TSTRUCT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)

#include <Windows.h>
#include <conio.h>

#define THREAD_FUNC unsigned int __stdcall

#elif defined linux

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <aio.h>
#include <errno.h>
#include <semaphore.h>
#include <dlfcn.h>

#define THREAD_FUNC void*

#else
#error Platform not supported
#endif

#define MAX_BUF_SIZE 256

enum {
  READ_THREAD = 5,
  WRITE_THREAD,
  THREAD_OPEN_FAILURE,
  THREAD_OPEN_SUCCESS,
  FILE_OPEN_FAILURE,
  UNEXPECTED_TYPE
};

struct thread {
#if defined(_WIN32) || defined(_WIN64)
  HANDLE hThread;
  unsigned int tid;
#elif defined linux
  pthread_t tid;
#else
#error Platform not supported
#endif
} thread_struct;

struct sync {
#if defined(_WIN32) || defined(_WIN64)
  HANDLE hMutex;
#elif defined linux
  pthread_mutex_t mutex;
#else
#error Platform not supported
#endif
} sync_struct;

typedef struct thread  thread_t;
typedef struct sync    sync_t;

#endif /* TSTRUCT_H_ */