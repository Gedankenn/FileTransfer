#include "server.h"
#include "file_handler.h"
#include "file_transfer_protocol.h"
#include "msocket.h"
#include "color.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Send pfile, its children and its following siblings (parents first). */
static int send_tree(int sfd, struct file_st* pfile, const char* root_path,
                     const char* root_name, long int* data_sent, long int total)
{
    for (; pfile != NULL; pfile = pfile->brother)
    {
        char rel[FILE_PATH_SIZE];
        /* relative path = root name + whatever follows the root path */
        int n = snprintf(rel, sizeof rel, "%s%s", root_name, pfile->path + strlen(root_path));
        if (n < 0 || n >= (int)sizeof rel)
            return BAD_PATH;

        int is_dir = pfile->type == DIRECTORY_E;
        if (!is_dir)
            printf("Sending file: %s (%li bytes)\n", rel, pfile->size);
        int ret = send_entry(sfd, is_dir, rel, pfile->path, is_dir ? 0 : (uint64_t)pfile->size);
        if (ret != OK)
        {
            printf("%sError sending %s%s\n", ANSI_RED, rel, ANSI_RESET);
            return ret;
        }
        if (!is_dir)
        {
            *data_sent += pfile->size;
            printf("%sSent %li of %li%s\n", ANSI_GREEN, *data_sent, total, ANSI_RESET);
        }
        if (is_dir && pfile->childrem)
        {
            ret = send_tree(sfd, pfile->childrem, root_path, root_name, data_sent, total);
            if (ret != OK)
                return ret;
        }
    }
    return OK;
}

int server(char *port, int buf_size, char* path)
{
    (void)buf_size;
    int sfd;
    int ret = 0;
    char root_path[FILE_PATH_SIZE];
    struct file_tree_st root;

    /* drop trailing separators so the root name is not empty */
    snprintf(root_path, sizeof root_path, "%s", path);
    size_t len = strlen(root_path);
    while (len > 1 && root_path[len - 1] == PATH_SEP)
        root_path[--len] = '\0';

    ret = read_dir(root_path, &root);
    if (ret == PATH_ERROR)
    {
        printf("%sWrong file path passed as parameter\n%s",ANSI_RED, ANSI_RESET);
        return ret;
    }
    if (ret < 0)
    {
        return ret;
    }

    printf("--------------- Initialized server side ------------------\n");
    printf("Port: %s\n",port);
    printf("----------------------------------------------------------\n");
    print_metadata(&root);

    sfd = create_socket(port);
    if(sfd < 0)
    {
        printf("%sError connecting to socket\n%s", ANSI_RED, ANSI_RESET);
        return ERROR;
    }

    /* root entry + every file and folder below it */
    uint32_t entries = 1 + root.files_count + root.folders_count;
    if (root.root->type == FILE_E)
        entries = 1;

    printf("%sSend root file metadata\n%s", ANSI_GREEN, ANSI_RESET);
    if (send_header(sfd, entries, (uint64_t)root.total_size) != OK)
    {
        printf("%sError sending root file metadata\n%s",ANSI_RED, ANSI_RESET);
        close(sfd);
        return ERROR;
    }

    printf("%sStart sending data\n%s",ANSI_GREEN, ANSI_RESET);
    long int data_sent = 0;
    struct file_st* top = root.root;
    int is_dir = top->type == DIRECTORY_E;
    ret = send_entry(sfd, is_dir, top->name, top->path, is_dir ? 0 : (uint64_t)top->size);
    if (ret == OK && !is_dir)
        data_sent = top->size;
    if (ret == OK && is_dir && top->childrem)
        ret = send_tree(sfd, top->childrem, root_path, top->name, &data_sent, root.total_size);
    close(sfd);

    if (ret != OK)
    {
        printf("Error sending data\n");
        return ERROR;
    }
    if (data_sent != root.total_size)
    {
        printf("%sWrong size sent%s\n", ANSI_RED, ANSI_RESET);
        return ERROR;
    }
    printf("%sTransfer finished: %li bytes%s\n", ANSI_GREEN, data_sent, ANSI_RESET);
    return SUCCESS;
}
