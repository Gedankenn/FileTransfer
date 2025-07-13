#include <stdio.h>
#include <stdlib.h>
#include "msocket.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    int ret = 0;
    char ip[256];
    ret = find_my_ip(4, ip);
    printf("My ip: %s\n",ip);

    return EXIT_SUCCESS;
}
