#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../../include/common.h"
#include "../../include/server.h"

client_t clients[MAX_CLIENTS];

void init_clients() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    clients[i].fd = -1;
    clients[i].state = STATE_NEW;
    memset(&clients[i].buffer, '\0', BUF_SIZE);
  }
}

int find_free_slot() {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].fd == -1) {
      return i;
    }
  }

  return -1;
}

int find_free_slot_by_fd(int fd) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].fd == fd) {
      return i;
    }
  }

  return -1;
}

int get_listener_socket(char *port) {
  int addrInfoRes, listenfd;
  int yes = 1;
  struct addrinfo hints, *serverInfo, *p;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;

  if ((addrInfoRes = getaddrinfo(NULL, port, &hints, &serverInfo)) != 0) {
    printf("Error getaddrinfo: %s", gai_strerror(addrInfoRes));
    return STATUS_ERROR;
  }

  for (p = serverInfo; p != NULL; p = p->ai_next) {
    if ((listenfd = socket(serverInfo->ai_family, serverInfo->ai_socktype,
                           serverInfo->ai_protocol)) == -1) {
      continue;
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
        -1) {
      continue;
    }

    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == -1) {
      continue;
    }
  }

  if (p == NULL) {
    printf("Server failed to bind socket");
    return STATUS_ERROR;
  }

  freeaddrinfo(serverInfo);

  if (listen(listenfd, BACKLOG) == -1) {
    perror("listen");
    return STATUS_ERROR;
  }

  return listenfd;
}

int poll_server(char *port) {
  int remotefd, listenfd, fdCount;
  struct sockaddr_storage remoteAddr;
  socklen_t remoteAddrLen;

  struct pollfd *pfds = calloc(FD_SIZE, sizeof(struct pollfd));
  if (pfds == NULL) {
    perror("calloc");
    return STATUS_ERROR;
  }

  listenfd = get_listener_socket(port);
  if (listenfd == STATUS_ERROR) {
    return STATUS_ERROR;
  }

  pfds[0].fd = listenfd;
  pfds[0].events = POLLIN;

  fdCount = 1;

  // get socket
  return STATUS_SUCCESS;
}
