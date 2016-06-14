#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include "fqueue.h"

int init_queue(fqueue_t **queue)
{
  (*queue)->out_buf = (char *)malloc(MAX_BUF_SIZE * sizeof(char));

  ZeroMemory((*queue)->out_buf, sizeof((*queue)->out_buf));
  if ((*queue)->out_buf == NULL) {
    return MEMORY_ALLOC_ERROR;
  }

  (*queue)->out_file_name = (char *)malloc(
                              MAX_FILE_NAME_SIZE * sizeof(char));
  if ((*queue)->out_file_name == NULL) {
    free((*queue)->out_buf);
    return MEMORY_ALLOC_ERROR;
  }

  (*queue)->lpOutOverlap.Offset = 0;
  (*queue)->lpOutOverlap.OffsetHigh = 0;
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

    temp->buf = (char *)malloc(MAX_BUF_SIZE * sizeof(char));
    if (temp->buf == NULL) {
      free(temp->file_name);
      free(temp);
      clear_queue(queue);
      return MEMORY_ALLOC_ERROR;
    }

    temp->bytes = 0;
    ZeroMemory(temp->buf, sizeof(temp->buf));

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
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;

  hFind = FindFirstFile(file_name, &FindFileData) ;

  if (hFind != INVALID_HANDLE_VALUE) {
    FindClose(hFind);
    return FILE_EXIST;
  } else {
    return FILE_NOT_EXIST;
  }
}

int create_file(const char *fname)
{
  HANDLE hFile;

  hFile = CreateFile(
            fname,
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
          );

  if (hFile == INVALID_HANDLE_VALUE) {
    printf("\nFile creation error.");
    return FILE_CREATE_FAILURE;
  } else {
    return FILE_CREATE_SUCCESS;
  }
}
