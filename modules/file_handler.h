#ifndef __FILE_HANDLER_H__ 
#define __FILE_HANDLER_H__

#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#define FILE_PATH_SIZE 1024
#define FILE_NAME_SIZE 256

enum
{
    SUCCESS = 0,
    ERROR = -1,
    NO_FILE_FOUND = -2,
};

typedef enum
{
    FILE_E,
    DIRECTORY_E
}file_type_e;

struct file_st
{
    struct file_st *parent;
    struct file_st *childrem;
    struct file_st *brother;
    file_type_e type;
    char name[FILE_NAME_SIZE];
    char path[FILE_PATH_SIZE];
    int depth;
    int size;
};

struct file_tree_st
{
    struct file_st* root;
    int total_size;
    int files_count;
    int folders_count;
};

int read_dir(char *path, struct file_tree_st* root);
int get_file_bin(struct file_st* file, unsigned char* bin_file);
bool create_dir(char* path);
bool create_file(char* path);

#endif //__FILE_HANDLER_H__
