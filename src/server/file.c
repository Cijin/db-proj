#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../include/common.h"
#include "../../include/file.h"

int open_db_file(char *filename) {
  int fd = open(filename, O_RDWR, 0644);
  if (fd == -1) {
    perror("open");
    return fd;
  }

  return fd;
}

int create_db_file(char *filename) {
  int fd = open(filename, O_RDONLY, 0644);
  if (fd != -1) {
    close(fd);
    printf("File already exists\n");
    return STATUS_ERROR;
  }

  fd = open(filename, O_RDWR | O_CREAT, 0644);
  if (fd == -1) {
    perror("create");
    return STATUS_ERROR;
  }

  return fd;
}
