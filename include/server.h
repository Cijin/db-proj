#ifndef SERVER_H
#define SERVER_H

#define BACKLOG 10
#define MAX_CLIENTS 256
#define BUF_SIZE 4096

typedef enum { STATE_NEW, STATE_CONNECTED, STATE_DISCONNECTED } state_e;

typedef struct {
  int fd;
  state_e state;
  char buffer[BUF_SIZE];
} client_t;

void poll_server(char *port);

#endif
