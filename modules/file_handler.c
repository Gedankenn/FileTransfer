#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include <unistd.h>

void init_file_node(struct file_st *f)
{
    f->parent = NULL;
    f->childrem = NULL;
    f->brother = NULL;
    f->size = 0;
}

void scan_dir(const char *path, int depth, struct file_st *file, struct file_tree_st *root)
{
    struct dirent *entry;
    DIR *dp = opendir(path);
    struct file_st *q;
    q = malloc(sizeof(struct file_st));
    init_file_node(q);

    if (dp == NULL) 
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)))
    {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        #ifdef DEBUG
        // Indentation for hierarchy
        for (int i = 0; i < depth; i++)
        {
            printf("  ");
        }
        printf("|-- %s\n", entry->d_name);
        #endif
        // Build new path
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        q->parent = file;
        file->childrem = q;
        snprintf(q->path, FILE_PATH_SIZE, "%s", fullpath);
        snprintf(q->name, FILE_NAME_SIZE, "%s", entry->d_name);

        // If it's a directory, recurse
        struct stat statbuf;
        if (stat(fullpath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
        {
            q->type = DIRECTORY_E;
            scan_dir(fullpath, depth + 1, q, root);
            root->folders_count++;
        }
        else
        {
            q->type = FILE_E;
            if(file->type == FILE_E)
            {
                q->brother = file;
                file->brother = q;
                root->files_count++;
                root->total_size = root->total_size + statbuf.st_size;
            }
        }
    }

    closedir(dp);
}

int read_dir(char *path, struct file_tree_st* root)
{
    char full_path[FILE_PATH_SIZE];
    getcwd(full_path, FILE_PATH_SIZE);
#ifdef DEBUG
    printf("--------------- Debug ------------------\n");
    printf("cwd: %s\n",full_path);
    printf("Contents of directory: %s\n", path);
    printf("----------------------------------------\n");
#endif
    struct file_st f;
    init_file_node(&f);
    root->root = &f;
    scan_dir(path, 0, &f, root);

    return SUCCESS;
}
