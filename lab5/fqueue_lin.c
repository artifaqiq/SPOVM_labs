#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "fqueue.h"

int init_queue(fqueue_t **queue)
{
  bzero((char *)&((*queue)->out_aio), sizeof(struct aiocb));

  (*queue)->out_buf = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
  if ((*queue)->out_buf == NULL) {
    return MEMORY_ALLOC_ERROR;
  }
  (*queue)->out_aio.aio_buf = (*queue)->out_buf;

  (*queue)->out_file_name = (char *)malloc(
                              MAX_FILE_NAME_SIZE * sizeof(char));
  if ((*queue)->out_file_name == NULL) {
    free((*queue)->out_buf);
    return MEMORY_ALLOC_ERROR;
  }

  (*queue)->out_aio.aio_nbytes = MAX_BUF_SIZE;
  return QUEUE_EMPTY;
}

int enqueue(const char *fname, fqueue_t **queue)
{
  struct list *temp;

  temp = (struct list *)malloc(sizeof(struct list));
  if (temp != NULL) {
    temp->file_name = (char *)malloc(
                        MAX_FILE_NAME_SIZE * sizeof(char));
    if (temp->file_name == NULL) {
      free(temp);
      clear_queue(queue);
      return MEMORY_ALLOC_ERROR;
    }

    bzero((char *)&(temp->aio), sizeof(struct aiocb));

    temp->buf = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
    if (temp->buf == NULL) {
      free(temp->file_name);
      free(temp);
      clear_queue(queue);
      return MEMORY_ALLOC_ERROR;
    }

    temp->aio.aio_buf = temp->buf;
    temp->aio.aio_nbytes = MAX_BUF_SIZE;
    temp->aio.aio_offset = 0;

    strcpy(temp->file_name, fname);

    temp->next = NULL;

    if ((*queue)->head == NULL) {
      (*queue)->head = temp;
    } else {
      (*queue)->tail->next = temp;
    }

    (*queue)->tail = temp;
  } else {
    return ENQUEUE_FAILURE;
  }
  return ENQUEUE_SUCCESS;
}

int dequeue(fqueue_t **queue)
{
  struct list *temp;

  if ((*queue)->head == (*queue)->tail) {
    free((*queue)->head->buf);
    free((*queue)->head->file_name);
    free((*queue)->head);
    (*queue)->tail = (*queue)->head = NULL;
    return DEQUEUE_SUCCESS;
  }

  temp = (*queue)->head->next;

  free((*queue)->head->file_name);
  free((*queue)->head->buf);
  free((*queue)->head);

  (*queue)->head = temp;
  return DEQUEUE_SUCCESS;
}

int clear_queue(fqueue_t **queue)
{
  while ((*queue)->head != (*queue)->tail
         || (*queue)->head != NULL) {
    dequeue(queue);
  }
  return DEQUEUE_SUCCESS;
}

void print_queue(fqueue_t *queue)
{
  struct list *temp;
  temp = queue->head;
  printf("\nQueue content: \n");

  if (temp == NULL) {
    printf("\n Queue is empty");
    return;
  }

  do {
    printf("\n%s", temp->file_name);
    temp = temp->next;
  } while (temp);
}

void delete_queue(fqueue_t **queue)
{
  free((*queue)->out_buf);
  free((*queue)->out_file_name);
  return;
}

int is_file_exist(const char *file_name)
{
  if(access(file_name, F_OK) != -1) {
    return FILE_EXIST;
  } else {
    return FILE_NOT_EXIST;
  }
}

int create_file(const char *fname)
{
  int handle;
  handle = open(fname, O_RDWR | O_CREAT, 0666);
  if (handle == -1) {
    return FILE_CREATE_FAILURE;
  } else {
    return FILE_CREATE_SUCCESS;
  }
}
