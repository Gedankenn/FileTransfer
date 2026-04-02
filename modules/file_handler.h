#ifndef __FILE_HANDLER_H__ 
#define __FILE_HANDLER_H__

#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEP '\\'
#else
#include <dirent.h>
#include <sys/stat.h>
#define PATH_SEP '/'
#endif

#define FILE_PATH_SIZE 1024
#define FILE_NAME_SIZE 256

enum
{
    SUCCESS = 0,
    ERROR = -1,
    NO_FILE_FOUND = -2,
    PATH_ERROR = -3,
};

typedef enum
{
    FILE_E,
    DIRECTORY_E
}file_type_e;

typedef struct file_node
{
    char name[FILE_NAME_SIZE];
    char path[FILE_PATH_SIZE];

    size_t size;

    file_type_e type;

    struct file_node *parent;

    struct file_node *first_child;
    struct file_node *next_sibling;

} file_node_t;

struct file_tree_st
{
    struct file_note_t* root;
    long  int total_size;
    int files_count;
    int folders_count;
};

enum{
    BYTES = 0,
    KBYTES,
    MBYTES,
    GBYTES,
    PBYTES
};

#endif //__FILE_HANDLER_H__
