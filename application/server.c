#include "server.h"


void server(char *port, int buf_size)
{
  char buf[buf_size];
  char data[buf_size];
  int nread = 0;
  int sfd = 0;
  init_socket(buf_size, AF_UNSPEC, 0);
  sfd = create_socket(port);
  printf("Server created\n");
  do {
    memset(data, 0, buf_size);
    nread = socket_read(sfd, buf);
    memcpy(data, buf, nread);
    printf("read: %s\n", data);
  } while (strncmp(data, EXIT, nread) != 0 && strlen(data) != 0);
}
