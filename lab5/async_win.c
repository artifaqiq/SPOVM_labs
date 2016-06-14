#include "async_win.h"
#include <windows.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

BOOL WINAPI DllMain (HANDLE hInst, DWORD dwReason, LPVOID IpReserved)
{
  BOOL bAllWentWell = TRUE;

  switch (dwReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }
  if (bAllWentWell) {
    return TRUE;
  } else {
    return FALSE;
  }
}

__declspec(dllexport) BOOL async_read(fqueue_t **queue)
{
  HANDLE hOpenFile;
  DWORD dwBytesRead;
  BOOL result = FALSE;

  hOpenFile = CreateFile (
                (*queue)->head->file_name,
                GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
              );

  if (hOpenFile == INVALID_HANDLE_VALUE) {
    printf("\nFile open error.");
    CloseHandle(hOpenFile);
    return FALSE;
  }

  (*queue)->head->hInFile = hOpenFile;
  (*queue)->head->lpInOverlap.hEvent = (*queue)->hReadEvent;
  (*queue)->head->lpInOverlap.Offset = 0;
  (*queue)->head->lpInOverlap.OffsetHigh = 0;

  printf("\nFile \"%s\" reading ... ", (*queue)->head->file_name);
  ReadFile(
    (*queue)->head->hInFile,
    (*queue)->head->buf,
    MAX_BUF_SIZE,
    &dwBytesRead,
    &((*queue)->head->lpInOverlap)
  );

  WaitForSingleObject((*queue)->head->lpInOverlap.hEvent, INFINITE);

  result = GetOverlappedResult(
             (*queue)->head->hInFile,
             &((*queue)->head->lpInOverlap),
             &dwBytesRead,
             FALSE
           );

  if (result) {
    printf("%lu bytes read!", dwBytesRead);
    (*queue)->head->bytes = dwBytesRead;
  } else {
    printf(" file read error.");
  }

  CloseHandle((*queue)->head->hInFile);
  return TRUE;
}

__declspec(dllexport) BOOL async_write(fqueue_t **queue)
{
  HANDLE hOpenFile;
  DWORD dwBytesWrite;
  BOOL result = FALSE;

  hOpenFile = CreateFile (
                (*queue)->out_file_name,
                GENERIC_WRITE,
                FILE_SHARE_WRITE,
                NULL,
                OPEN_ALWAYS,
                FILE_FLAG_OVERLAPPED,
                NULL
              );

  if (hOpenFile == INVALID_HANDLE_VALUE) {
    printf("\nFile open error.");
    CloseHandle(hOpenFile);
    return FALSE;
  }

  (*queue)->hOutFile = hOpenFile;
  (*queue)->lpOutOverlap.hEvent = (*queue)->hWriteEvent;
  (*queue)->lpOutOverlap.Offset = SetFilePointer(
                                    hOpenFile, 0, NULL, FILE_END);
  strcpy((*queue)->out_buf, (*queue)->head->buf);

  printf("\nWriting ... ");
  WriteFile(
    (*queue)->hOutFile,
    (*queue)->out_buf,
    (*queue)->head->bytes,
    NULL,
    &((*queue)->lpOutOverlap)
  );

  WaitForSingleObject((*queue)->hWriteEvent, INFINITE);

  result = GetOverlappedResult(
             (*queue)->hOutFile,
             &((*queue)->lpOutOverlap),
             &dwBytesWrite,
             TRUE
           );

  if (result) {
    printf("%lu bytes writed!", dwBytesWrite);
  } else {
    printf(" write file error.");
  }
  CloseHandle((*queue)->hOutFile);
  return TRUE;
}

#ifdef __cplusplus
}
#endif