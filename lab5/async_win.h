#ifndef ASYNC_WIN_H_
#define ASYNC_WIN_H_

#include <windows.h>
#include "fqueue.h"

#ifdef __cplusplus
extern "C"
{
#endif

__declspec(dllexport) BOOL async_read(fqueue_t **);
__declspec(dllexport) BOOL async_write(fqueue_t **);

#ifdef __cplusplus
}
#endif

#endif /* ASYNC_WIN_H_ */