#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../include/common.h"
#include "../../include/parse.h"

void list_employees(struct dbheader_t *dbHeader, struct employee_t *employees) {
  if (dbHeader->count == 0) {
    printf("Db is empty. Add an employee using -a\n");
    return;
  }

  for (int i = 0; i < dbHeader->count; i++) {
    printf("Employee: %d\n", i + 1);
    printf("\tName: %s\n", employees[i].name);
    printf("\tAddress: %s\n", employees[i].address);
    printf("\tHours: %d\n", employees[i].hours);
  }

  return;
}

int add_empployee(struct dbheader_t *dbHeader, struct employee_t *employees,
                  char *addString) {
  char *name = strtok(addString, ",");
  char *address = strtok(NULL, ",");
  int hours = strtol(strtok(NULL, ","), NULL, 10);

  strncpy(employees[dbHeader->count - 1].name, name,
          sizeof(employees[dbHeader->count - 1].name));
  strncpy(employees[dbHeader->count - 1].address, address,
          sizeof(employees[dbHeader->count - 1].address));
  employees[dbHeader->count - 1].hours = hours;

  return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbHeader,
                   struct employee_t **employeesOut) {

  if (fd == -1) {
    printf("Error: verify_db_header: invlalid fd\n");
    return STATUS_ERROR;
  }

  int employeesCount = dbHeader->count;

  struct employee_t *employees =
      calloc(employeesCount, sizeof(struct employee_t));
  if (employees == NULL) {
    perror("read_employees: Malloc");
    return STATUS_ERROR;
  }

  read(fd, employees, sizeof(struct employee_t) * employeesCount);

  for (int i = 0; i < employeesCount; i++) {
    employees[i].hours = ntohl(employees[i].hours);
  }

  *employeesOut = employees;

  return STATUS_SUCCESS;
}

void save_db(int fd, struct dbheader_t *dbHeader,
             struct employee_t *employees) {
  if (fd == -1) {
    printf("Error: save_db: invlalid fd\n");
    return;
  }

  int count = dbHeader->count;

  dbHeader->magic = htonl(dbHeader->magic);
  dbHeader->filesize =
      htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t) * count);
  dbHeader->version = htons(dbHeader->version);
  dbHeader->count = htons(dbHeader->count);

  lseek(fd, 0, SEEK_SET);

  if (write(fd, dbHeader, sizeof(struct dbheader_t)) == -1) {
    perror("write header");
    return;
  }

  for (int i = 0; i < count; i++) {
    employees[i].hours = htonl(employees[i].hours);
    if (write(fd, &employees[i], sizeof(struct employee_t)) == -1) {
      perror("write employee");
      return;
    }
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
