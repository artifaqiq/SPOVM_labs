#include <stdio.h>
#include <errno.h>
#include <aio.h>
#include "fqueue.h"

int async_read(fqueue_t **queue);
int async_write(fqueue_t **queue);

int async_read(fqueue_t **queue)
{
  int fd, ret;

  fd = open((*queue)->head->file_name, O_RDONLY);
  if (fd < 0) {
    printf("\nFile open error.");
    return -1;
  }

  (*queue)->head->aio.aio_fildes = fd;
  ret = aio_read(&((*queue)->head->aio));
  if (ret < 0) {
    close(fd);
    printf("\nSomething went wrong with file [aio_read].");
    return -1;
  }

  printf("\nFile \"%s\" reading ... ", (*queue)->head->file_name);
  while (aio_error(&((*queue)->head->aio)) == EINPROGRESS) {
    continue;
  }

  if ((ret = aio_return(&((*queue)->head->aio))) > 0) {
    close(fd);
    printf("%d bytes readed!", ret);
    return ret;
  } else {
    printf("\nRead failed.");
    return -1;
  }
  return 0;
}

int async_write(fqueue_t **queue)
{
  int fd, ret;

  fd = open((*queue)->out_file_name, O_WRONLY | O_APPEND);
  if (fd < 0) {
    printf("\nFile open error.");
    return -1;
  }

  (*queue)->out_aio.aio_fildes = fd;
  (*queue)->out_aio.aio_nbytes = strlen((*queue)->head->buf);
  strcpy((*queue)->out_buf, (*queue)->head->buf);
  aio_write(&((*queue)->out_aio));

  printf("\nWriting ... ");
  while(aio_error(&((*queue)->out_aio)) == EINPROGRESS) {
    continue;
  }

  if ((ret = aio_return(&((*queue)->out_aio))) > 0) {
    close(fd);
    printf("%d bytes writed!\n", ret);
    return ret;
  } else {
    close(fd);
    printf("\nWrite failed.");
    return -1;
  }
  return 0;
}
