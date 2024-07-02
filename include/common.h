#include <stdint.h>

#ifndef COMMON_H
#define COMMON_H

#define STATUS_ERROR -1
#define STATUS_SUCCESS 0

#define PROTO 100
#define PORT "8080"

typedef enum { MSG_HELLO_REQ, MSG_HELLO_RES } dbproto_type_e;

typedef struct {
  dbproto_type_e type;
  uint16_t len;
} dbproto_hdr_t;

#endif
