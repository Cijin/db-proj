#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/parse.h"

void save_db(int fd, struct dbheader_t *dbHeader) {
  if (fd == -1) {
    printf("Error: save_db: invlalid fd\n");
    return;
  }

  dbHeader->magic = htonl(dbHeader->magic);
  dbHeader->filesize = htonl(dbHeader->filesize);
  dbHeader->version = htons(dbHeader->version);
  dbHeader->count = htons(dbHeader->count);

  lseek(fd, 0, SEEK_SET);

  if (write(fd, dbHeader, sizeof(struct dbheader_t)) == -1) {
    perror("write");
    return;
  }

  return;
}

int create_db_header(struct dbheader_t **headerOut) {
  struct dbheader_t *header = malloc(sizeof(struct dbheader_t));
  if (header == NULL) {
    perror("create db malloc");
    return STATUS_ERROR;
  }

  header->count = 0;
  header->version = DB_VERSION;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;

  return STATUS_SUCCESS;
}

int verify_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd == -1) {
    printf("Error: verify_db_header: invlalid fd\n");
    return STATUS_ERROR;
  }

  struct dbheader_t *header = malloc(sizeof(struct dbheader_t));
  if (header == NULL) {
    perror("verify db malloc");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(struct dbheader_t)) !=
      sizeof(struct dbheader_t)) {
    perror("verify db read");
    free(header);
    return STATUS_ERROR;
  }

  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if (header->magic != HEADER_MAGIC) {
    printf("Invalid file type: invlalid magic\n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->version != DB_VERSION) {
    printf("Invalid db version\n");
    free(header);
    return STATUS_ERROR;
  }

  struct stat statbuf = {0};
  if (fstat(fd, &statbuf) == -1) {
    perror("fstat");
    free(header);
    return STATUS_ERROR;
  }

  if (header->filesize != statbuf.st_size) {
    printf("File seems corrupted\n");
    free(header);
    return STATUS_ERROR;
  }

  *headerOut = header;

  return STATUS_SUCCESS;
}
