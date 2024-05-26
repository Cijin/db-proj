#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/file.h"

int open_file_rw(char *filename) {
  int fd = open(filename, O_RDWR);

  if (fd == -1) {
    perror("open");
    return fd;
  }

  return fd;
}
