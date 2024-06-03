#ifndef PARSE_H
#define PARSE_H

// CCDB
#define HEADER_MAGIC 0x43434442
#define DB_VERSION 0x1

struct dbheader_t {
  unsigned int magic;
  unsigned short version;
  unsigned short count;
  unsigned int filesize;
};

struct employee_t {
  char name[256];
  char address[256];
  unsigned int hours;
};

int create_db_header(struct dbheader_t **headerOut);
int verify_db_header(int fd, struct dbheader_t **headerOut);
void save_db(int fd, struct dbheader_t *dbHeader, struct employee_t *employees);

int read_employees(int fd, struct dbheader_t *dbHeader,
                   struct employee_t **employeesOut);
int add_empployee(struct dbheader_t *dbHeader, struct employee_t *employees,
                  char *addString);
void list_employees(struct dbheader_t *dbHeader, struct employee_t *employees);

#endif
