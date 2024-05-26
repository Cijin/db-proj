#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n - create a new database file\n");
  printf("\t -f - (required) path to database file\n");
}

int main(int argc, char *argv[]) {
  int c = 0;
  bool newfile = false;
  char *filepath = NULL;

  while ((c = getopt(argc, argv, "nf:")) != -1) {
    switch (c) {
    case 'n':
      newfile = true;
      break;

    case 'f':
      filepath = optarg;
      break;

    case '?':
      printf("Unknown option -%c\n", c);
      break;

    default:
      return -1;
    }
  }

  if (filepath == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);

    return -1;
  }

  // open file
  return 0;
}
