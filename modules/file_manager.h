#ifndef __FILE_MANAGER_H__ 
#define __FILE_MANAGER_H__

#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#define FILE_PATH_SIZE 1024
#define FILE_NAME_SIZE 128

enum
{
    NO_FILE_FOUND = -1,
    ERROR = 0,
    SUCCESS = 1,
};

typedef enum
{
    FILE_E,
    DIRECTORY_E
}file_type_e;

struct file_st
{
    char name[FILE_NAME_SIZE];
    char path[FILE_PATH_SIZE];
    int size;
    file_type_e type;
    struct file_st *parent;
    struct file_st *childrem;
    struct file_st *brother;
};

struct file_tree_st
{
    struct file_st root;
    int total_size;
    int files_count;
    int folders_count;
};

#endif //__FILE_MANAGER_H__
