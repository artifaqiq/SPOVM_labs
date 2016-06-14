#ifndef FQUEUE_H_
#define FQUEUE_H_

#include "tstruct.h"

#define MAX_FILE_NAME_SIZE 256
#define MEMORY_ALLOC_ERROR -101

enum {
  ENQUEUE_SUCCESS = 100,
  ENQUEUE_FAILURE,
  DEQUEUE_SUCCESS,
  QUEUE_EMPTY,
  FILE_EXIST,
  FILE_NOT_EXIST,
  FILE_CREATE_FAILURE,
  FILE_CREATE_SUCCESS
};

#if defined(_WIN32) || defined(_WIN64)

struct list {
  OVERLAPPED lpInOverlap;
  HANDLE hInFile;
  DWORD bytes;
  char *file_name;
  char *buf;
  struct list *next;
};

struct fqueue {
  OVERLAPPED lpOutOverlap;
  HANDLE hOutFile;
  HANDLE hReadEvent;
  HANDLE hWriteEvent;
  char *out_buf;
  char *out_file_name;
  struct list *head;
  struct list *tail;
} aio_queue;

#elif defined linux

struct list {
  char *file_name;
  char *buf;
  struct aiocb aio;
  struct list *next;
};

struct fqueue {
  char *out_buf;
  char *out_file_name;
  struct aiocb out_aio;
  struct list *head;
  struct list *tail;
} aio_queue;

#else
#error Platform not supported
#endif

typedef struct fqueue fqueue_t;

int  init_queue(fqueue_t **);
int  enqueue(const char *, fqueue_t **);
int  dequeue(fqueue_t **);
int  clear_queue(fqueue_t **);
void print_queue(fqueue_t *);
void delete_queue(fqueue_t **);
int  is_file_exist(const char *);
int  create_file(const char *);

#endif /* FQUEUE_H_ */