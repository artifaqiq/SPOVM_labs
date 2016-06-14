#include "fqueue.h"
#include "thread.h"
#include "tstruct.h"

THREAD_FUNC start_read(void *args)
{
  void* library;
  int (*async_read)(fqueue_t **);
  sem_t *read_sem;
  sem_t *write_sem;

  fqueue_t **queue;

  queue = (fqueue_t **)args;
  read_sem = sem_open("/read", O_CREAT, 0666, 0);
  write_sem = sem_open("/write", O_CREAT, 0666, 0);

  library = dlopen(SHARED_LIB_NAME, RTLD_LAZY);
  if (!library) {
    printf("\n\nLibrary link failure.");
    clear_queue(queue);
    exit(EXIT_FAILURE);
  }

  *(void **)(&async_read) = dlsym(library, "async_read");

  for ( ; (*queue)->head != NULL; ) {
    async_read(queue);
    sem_post(read_sem);
    sem_wait(write_sem);
  }

  dlclose(library);
  return NULL;
}

THREAD_FUNC start_write(void *args)
{
  void* library;
  int (*async_write)(fqueue_t **);
  sem_t *read_sem;
  sem_t *write_sem;

  fqueue_t **queue;

  queue = (fqueue_t **)args;
  read_sem = sem_open("/read", O_CREAT, 0666, 0);
  write_sem = sem_open("/write", O_CREAT, 0666, 0);

  library = dlopen(SHARED_LIB_NAME, RTLD_LAZY);
  if (!library) {
    printf("\n\nLibrary link failure.");
    clear_queue(queue);
    exit(EXIT_FAILURE);
  }

  *(void **)(&async_write) = dlsym(library, "async_write");
  for ( ; (*queue)->head != NULL; ) {
    sem_wait(read_sem);
    async_write(queue);
    dequeue(queue);
    sem_post(write_sem);
  }

  sem_close(read_sem);
  sem_close(write_sem);
  sem_unlink("/read");
  sem_unlink("/write");
  dlclose(library);
  return NULL;
}

int thread_open(thread_t *thread, fqueue_t **queue, int iothread)
{
  int handle;

  if (iothread == READ_THREAD) {
    handle = pthread_create(&(thread->tid), NULL,
                            &start_read, (void *)queue);
  } else if (iothread == WRITE_THREAD) {
    handle = pthread_create(&(thread->tid), NULL,
                            &start_write, (void *)queue);
  } else {
    return UNEXPECTED_TYPE;
  }

  if (handle != 0) {
    return THREAD_OPEN_FAILURE;
  } else {
    return THREAD_OPEN_SUCCESS;
  }
}

void close_thread(thread_t *thread)
{
  pthread_join(thread->tid, NULL);
  return;
}
