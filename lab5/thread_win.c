#include <process.h>
#include <Windows.h>

#include "fqueue.h"
#include "thread.h"
#include "tstruct.h"

BOOL (*async_read)(fqueue_t **queue);
BOOL (*async_write)(fqueue_t **queue);

THREAD_FUNC start_read(void *args)
{
  HMODULE hDynamicLib;

  HANDLE hReadEvent;
  HANDLE hWriteEvent;

  fqueue_t **queue;

  queue = (fqueue_t **)args;

  hReadEvent = CreateEvent(NULL, FALSE, FALSE, "NewReadEvent");
  hWriteEvent = CreateEvent(NULL, FALSE, FALSE, "NewWriteEvent");

  hDynamicLib = LoadLibrary(DLL_NAME);
  if (!hDynamicLib) {
    printf("\n\nLibrary link failure.");
    clear_queue(queue);
    exit(EXIT_FAILURE);
  }

  async_read = (BOOL (*) (fqueue_t **))GetProcAddress(
                 hDynamicLib, "async_read");

  for ( ; (*queue)->head != NULL; ) {
    async_read(queue);
    SetEvent(hReadEvent);
    WaitForSingleObject(hWriteEvent, INFINITE);
  }

  FreeLibrary(hDynamicLib);
  return NULL;
}

THREAD_FUNC start_write(void *args)
{
  HMODULE hDynamicLib;

  HANDLE hReadEvent;
  HANDLE hWriteEvent;

  fqueue_t **queue;

  queue = (fqueue_t **)args;

  hReadEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "NewReadEvent");
  hWriteEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "NewWriteEvent");

  hDynamicLib = LoadLibrary(DLL_NAME);
  if (!hDynamicLib) {
    printf("\n\nLibrary link failure.");
    clear_queue(queue);
    exit(EXIT_FAILURE);
  }

  async_write = (BOOL (*) (fqueue_t **))GetProcAddress(
                  hDynamicLib, "async_write");

  for ( ; (*queue)->head != NULL; ) {
    WaitForSingleObject(hReadEvent, INFINITE);
    async_write(queue);
    dequeue(queue);
    SetEvent(hWriteEvent);
  }

  FreeLibrary(hDynamicLib);
  return NULL;
}

int thread_open(thread_t *thread, fqueue_t **queue, int iothread)
{
  if (iothread == READ_THREAD) {
    thread->hThread = (HANDLE)_beginthreadex(
                        NULL,
                        0,
                        &start_read,
                        (void *)queue,
                        0,
                        &(thread->tid)
                      );
  } else if (iothread == WRITE_THREAD) {
    thread->hThread = (HANDLE)_beginthreadex(
                        NULL,
                        0,
                        &start_write,
                        (void *)queue,
                        0,
                        &(thread->tid)
                      );
  } else {
    return UNEXPECTED_TYPE;
  }

  if (thread->hThread == 0) {
    return THREAD_OPEN_FAILURE;
  } else {
    return THREAD_OPEN_SUCCESS;
  }
}

void close_thread(thread_t *thread)
{
  CloseHandle(thread->hThread);
  WaitForSingleObject(thread->hThread, INFINITE);
  return;
}
