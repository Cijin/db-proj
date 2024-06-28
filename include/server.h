#ifndef SERVER_H
#define SERVER_H

#define BACKLOG 10
#define FD_SIZE 10
#define MAX_CLIENTS 256
#define BUF_SIZE 4096
#define PORT "8080"
#define BACKLOG 10
#define EXIT_ERROR -1

typedef enum { STATE_NEW, STATE_CONNECTED, STATE_DISCONNECTED } state_e;

typedef struct {
  int fd;
  state_e state;
  char buffer[BUF_SIZE];
} client_t;

int poll_server(char *port);

#endif
