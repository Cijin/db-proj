#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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
    printf("Failed to bind socket");
    return STATUS_ERROR;
  }

  freeaddrinfo(serverInfo);

  if (listen(listenfd, BACKLOG) == -1) {
    perror("listen");
    return STATUS_ERROR;
  }

  return listenfd;
}

void poll_server(char *port) {
  int remotefd, listenfd, fdCount, freeSlot;
  struct sockaddr_storage remoteAddr;
  socklen_t remoteAddrLen;

  struct pollfd *pfds = calloc(MAX_CLIENTS, sizeof(struct pollfd));
  if (pfds == NULL) {
    perror("calloc");
    exit(STATUS_ERROR);
  }

  listenfd = get_listener_socket(port);
  if (listenfd == STATUS_ERROR) {
    exit(STATUS_ERROR);
  }

  pfds[0].fd = listenfd;
  pfds[0].events = POLLIN;

  fdCount = 1;

  init_clients();

  while (1) {
    // update fd set to include clients that are connected
    int ii = 1; // offset by 1 for listenfd
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (clients[i].fd != -1) {
        pfds[ii].fd = clients[i].fd;
        pfds[ii].events = POLLIN;
      }
    }

    // keep track of clients to serve
    int n_events = poll(pfds, fdCount, -1);
    if (n_events == -1) {
      perror("poll");
      exit(STATUS_ERROR);
    }

    // check if any of the returned events (revents) have data
    // to read, i.e. the POLLIN bit is set
    if (pfds[0].revents & POLLIN) {
      if ((remotefd = accept(listenfd, (struct sockaddr *)&remoteAddr,
                             &remoteAddrLen)) == -1) {
        perror("accept");
        continue;
      }

      freeSlot = find_free_slot();
      if (freeSlot == -1) {
        printf("Server full: closing new connection\n");
        close(remotefd);
      } else {
        clients[freeSlot].fd = remotefd;
        clients[freeSlot].state = STATE_CONNECTED;

        fdCount++;
        printf("Slot %d assigned to clientfd %d", freeSlot, remotefd);
      }

      n_events--;
    }

    // skipping socket fd fds[0]
    for (int i = 1; i <= fdCount && n_events > 0; i++) {
      if (pfds[i].revents & POLLIN) {
        n_events--;

        int fd = pfds[i].fd;
        int slot = find_free_slot_by_fd(fd);

        ssize_t bytes_read =
            read(fd, clients[slot].buffer, sizeof(clients[slot].buffer));

        if (bytes_read <= 0) {
          // closed connection
          if (close(fd) == -1) {
            perror("close");
            if (slot == -1) {
              printf("Attempted to close non-existent fd\n");
            } else {
              clients[slot].fd = -1;
              clients[slot].state = STATE_DISCONNECTED;
              fdCount--;

              printf("Client disconnected or error\n");
            }
          }
        } else {
          // TODO:
          // check header
          // write info back to client
          printf("Recieved from client %d: %s\n", fd, clients[slot].buffer);
        }
      }
    }
  }
}
