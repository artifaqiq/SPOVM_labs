#ifndef THREAD_H_
#define THREAD_H_

#include "fqueue.h"

#define SHARED_LIB_NAME "./async_lin.so"
#define DLL_NAME "async_win.dll"

THREAD_FUNC start_read(void* args);
THREAD_FUNC start_write(void *args);

int  thread_open(thread_t *, fqueue_t **, int func_type);
void close_thread(thread_t *);

#endif /* THREAD_H_ */