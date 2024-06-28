#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../../include/common.h"
#include "../../include/server.h"

int poll_server(char *port) {
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

  // poll and accept connections

  return STATUS_SUCCESS;
}
