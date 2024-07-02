#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../include/common.h"

void send_hello(int fd) {
  char buf[4096] = {0};
  dbproto_hdr_t *hdr = buf;

  hdr->len = htons(1);
  hdr->type = htonl(MSG_HELLO_REQ);

  if (write(fd, buf, sizeof(dbproto_hdr_t)) == -1) {
    perror("write");
    return;
  }

  printf("Server connected to v1\n");
}

int main(int argc, char *argv[]) {
  int sfd;
  struct addrinfo hints, *clientInfo, *p;
  int addrRes;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  if ((addrRes = getaddrinfo(NULL, PORT, &hints, &clientInfo)) != 0) {
    printf("Client getaddrinfo err: %s", gai_strerror(addrRes));
    exit(STATUS_ERROR);
  }

  for (p = clientInfo; p != NULL; p = p->ai_next) {
    if ((sfd = socket(clientInfo->ai_family, clientInfo->ai_socktype,
                      clientInfo->ai_protocol)) == -1)
      continue;

    if (connect(sfd, clientInfo->ai_addr, clientInfo->ai_addrlen) != -1)
      break;

    close(sfd);
  }

  if (p == NULL) {
    printf("Client: unable to connect");
    exit(STATUS_ERROR);
  }

  freeaddrinfo(clientInfo);

  send_hello(sfd);

  return 0;
}
