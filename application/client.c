#include "client.h"
#include "file_handler.h"
#include "msocket.h"
#include "file_transfer_protocol.h"
#include "color.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int client(char* host, char* port, int buf_size, char* dest_dir)
{
    (void)buf_size;
    int sfd = 0;
    uint32_t entries = 0;
    uint64_t total = 0;

    if (dest_dir == NULL)
        dest_dir = ".";
    if (!create_dir(dest_dir))
        return ERROR;

    sfd = connect_socket(host, port);
    if(sfd < 0)
    {
        printf("%sError connecting to socket\n%s",ANSI_RED, ANSI_RESET);
        return ERROR;
    }

    printf("%sWaiting for root file metadata\n%s",ANSI_GREEN, ANSI_RESET);
    if (recv_header(sfd, &entries, &total) != OK)
    {
        printf("%sDidnt received the root file metadata from the server\n%s",ANSI_RED, ANSI_RESET);
        close(sfd);
        return ERROR;
    }
    printf("%sEntries: %u, total size: %llu bytes%s\n", ANSI_MAGENTA, entries,
           (unsigned long long)total, ANSI_RESET);

    unsigned long long received = 0;
    for (uint32_t i = 0; i < entries; i++)
    {
        long long n = recv_entry(sfd, dest_dir);
        if (n < 0)
        {
            printf("%sTransfer failed (error %lld)%s\n", ANSI_RED, n, ANSI_RESET);
            close(sfd);
            return ERROR;
        }
        received += (unsigned long long)n;
        printf("%sReceived %llu of %llu%s\n", ANSI_GREEN, received,
               (unsigned long long)total, ANSI_RESET);
    }
    close(sfd);

    if (received != total)
    {
        printf("%sWrong size received%s\n", ANSI_RED, ANSI_RESET);
        return ERROR;
    }
    printf("%sTransfer finished into %s%s\n", ANSI_GREEN, dest_dir, ANSI_RESET);
    return SUCCESS;
}
