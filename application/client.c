#include "client.h"

void client(char *port, int buf_size)
{
  char buf[buf_size];
  int sfd = 0;
  char host[256];

  printf("Enter desired ip: ");
  fgets(host, 256, stdin);
  printf("host: %s", host);
  host[strlen(host) - 1] = '\0';
  init_socket(buf_size, AF_UNSPEC, 0);
  sfd = connect_socket(host, port);
  do {
    fgets(buf, buf_size, stdin);
    printf("write: %s\n", buf);
    socket_write(sfd, buf, strlen(buf));
  } while (strcmp(buf, EXIT) != 0 && strlen(buf) - 1 != 0);
}

