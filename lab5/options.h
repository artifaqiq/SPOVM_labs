#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdio.h>
#include <getopt.h>

static const struct option long_options[] = {
  {"file",    required_argument, 0, 'f'},
  {"create",  required_argument, 0, 'c'},
  {"output",  required_argument, 0, 'o'},
  {"start",   no_argument,       0, 's'},
  {"help",    no_argument,       0, 'h'},
  {0,         0,                 0, 0  }
};

void print_usage()
{
  printf("You are in help menu, there are some commands,"
         "that you can use.\n");
  printf("Usage: \n");
  printf("\n     -h, --help               display help menu (this).");
  printf("\n     -f, --file   [name]      open file with \"name\"");
  printf("\n     -o, --output [name]      choose output file with \"name\".");
  printf("\n     -c, --create [name]      create output file with \"name\","
         "\n\t\t\t      if it don't already exist");

  printf("\n     -s, --start              start i/o operations.\n\n");
  return;
}

#endif /* OPTIONS_H_ */