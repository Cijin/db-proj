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

int create_db_header(struct dbheader_t **headerOut);
int verify_db_header(int fd, struct dbheader_t **headerOut);
void save_db(int fd, struct dbheader_t *dbHeader);

#endif
