#include "fqueue.h"
#include "options.h"
#include "tstruct.h"
#include "thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int handle;
  int opt;
  int long_index = 0;
  fqueue_t *queue;
  thread_t read;
  thread_t write;
  queue = &aio_queue;

  init_queue(&queue);
  while ((opt = getopt_long(argc, argv,"f:c:o:h:s",
                            long_options, &long_index )) != -1) {

    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 's': {
      thread_open(&read, &queue, READ_THREAD);
      thread_open(&write, &queue, WRITE_THREAD);
      getchar();
      close_thread(&read);
      close_thread(&write);
      break;
    }
    case 'f': {
      handle = is_file_exist(optarg);
      if (handle == FILE_NOT_EXIST) {
        printf("\nFIle with name \"%s\" not found!",
               optarg);
      } else {
        enqueue(optarg, &queue);
      }
      break;
    }
    case 'c': {
      create_file(optarg);
      break;
    }
    case 'o': {
      strcpy(queue->out_file_name, optarg);
      break;
    }
    case 'h': {
      print_usage();
      return EXIT_SUCCESS;
    }
    default: {
      print_usage();
      return EXIT_FAILURE;
    }
    }
  }

  clear_queue(&queue);
  delete_queue(&queue);
  printf("\n\nPress any key to continue . . .");
  getchar();
  return EXIT_SUCCESS;
}