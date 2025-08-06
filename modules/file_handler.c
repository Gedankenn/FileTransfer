#include <stdio.h>
#include <string.h>
#include "file_handler.h"

int read_dir(char *path, struct file_st* file)
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_status;
    char full_path[FILE_PATH_SIZE];

    dir = opendir(path);
    if(dir == NULL)
    {
        perror("No file found\n");
        return NO_FILE_FOUND;
    }

#ifdef DEBUG
    printf("Contents of directory: %s\n", path);
    printf("%-30s %s\n", "Name", "Type");
    printf("----------------------------------------\n");
#endif

    struct file_st *pn, *pb = NULL;
    entry = readdir(dir);
    while (entry != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || (strcmp(entry->d_name, "..") == 0))
        {
            continue;
        }

        // Create fullpath for file or folder 
        snprintf(full_path, FILE_PATH_SIZE, "%s/%s",path,entry->d_name);
        snprintf(file->path, FILE_PATH_SIZE, "%s",path);
        snprintf(file->name, FILE_NAME_SIZE, "%s",entry->d_name);

        //Get file/folder status 
        if(stat(full_path, &file_status) == 0)
        {
            #ifdef DEBUG 
            printf("%-30s Directory\n",entry->d_name);
            #endif
            file->parent = pb;
            file->type = DIRECTORY_E;
        }


    }

    return SUCCESS;
}
