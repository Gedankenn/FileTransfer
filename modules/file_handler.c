#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "file_handler.h"
#include <unistd.h>

int read_dir(char *path, struct file_st* file)
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_status;
    char full_path[FILE_PATH_SIZE];
    getcwd(full_path, FILE_PATH_SIZE);

#ifdef DEBUG
    printf("--------------- Debug ------------------\n");
    printf("cwd: %s\n",full_path);
    printf("Contents of directory: %s\n", path);
    printf("----------------------------------------\n");
#endif

    dir = opendir(path);
    if(dir == NULL)
    {
        perror("No file found\n");
        return NO_FILE_FOUND;
    }

    struct file_st *pb = NULL;
    struct file_tree_st f;
    f.root = file;
    entry = readdir(dir);
    while (entry != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || (strcmp(entry->d_name, "..") == 0))
        {
            entry = readdir(dir);
            continue;
        }

        // Create fullpath for file or folder 
        snprintf(full_path, FILE_PATH_SIZE, "%s/%s", path, entry->d_name);
        snprintf(file->path, FILE_PATH_SIZE, "%s", path);
        snprintf(file->name, FILE_PATH_SIZE, "%s", entry->d_name);

        //Get file/folder status 
        if(stat(full_path, &file_status) == 0)
        {
            if (S_ISDIR(file_status.st_mode))
            {
                #ifdef DEBUG 
                printf("%-30s Directory\n",entry->d_name);
                #endif
                file->parent = pb;
                file->type = DIRECTORY_E;
            }
            else
            {
                #ifdef DEBUG 
                printf("%-30s File\n",entry->d_name);
                #endif
                file->type = FILE_E;
            }
        }
        entry = readdir(dir);

    }

    return SUCCESS;
}
