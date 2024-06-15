#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../include/common.h"
#include "../../include/file.h"
#include "../../include/parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n - create a new database file\n");
  printf("\t -f - (required) path to database file\n");
  printf("\t -a - add an employee in the format \"name, address, hours\"\n");
  printf("\t -l - list current list of employees");
  printf("\t -p - port to run the server on");
}

int main(int argc, char *argv[]) {
  int c = 0;
  int dbfd = -1;
  int port;
  struct dbheader_t *dbHeader = NULL;
  struct employee_t *employees = NULL;
  bool newfile = false;
  bool listEmployees = false;
  char *filepath = NULL;
  char *portarg = NULL;
  char *addString = NULL;

  while ((c = getopt(argc, argv, "nlf:a:p:")) != -1) {
    switch (c) {
    case 'n':
      newfile = true;
      break;

    case 'l':
      listEmployees = true;
      break;

    case 'f':
      filepath = optarg;
      break;

    case 'a':
      addString = optarg;
      break;

    case 'p':
      portarg = optarg;
      port = atoi(portarg);
      if (port == 0) {
        printf("Bad port: %s\n", portarg);
      }
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

  if (newfile) {
    dbfd = create_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to create db file\n");
      return -1;
    }

    if (create_db_header(&dbHeader) == STATUS_ERROR) {
      printf("Unable to create db header\n");
      return -1;
    }
  } else {
    dbfd = open_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to open db file\n");
      return -1;
    }

    if (verify_db_header(dbfd, &dbHeader) == STATUS_ERROR) {
      printf("Invalid db header\n");
      return -1;
    }
  }

  if (read_employees(dbfd, dbHeader, &employees) == STATUS_ERROR) {
    printf("Failed to read employees\n");
    return -1;
  }

  if (addString) {
    dbHeader->count = ++dbHeader->count;

    employees = realloc(employees, sizeof(struct employee_t) * dbHeader->count);

    if (employees == NULL) {
      perror("realloc");
      return -1;
    }

    add_empployee(dbHeader, employees, addString);
  }

  if (listEmployees) {
    list_employees(dbHeader, employees);
  }

  save_db(dbfd, dbHeader, employees);
  return 0;
}
