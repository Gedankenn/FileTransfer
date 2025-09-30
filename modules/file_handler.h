#ifndef __FILE_HANDLER_H__ 
#define __FILE_HANDLER_H__

#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_SEP '/'
#else
#include <dirent.h>
#include <sys/stat.h>
#define PATH_SEP '\\'
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

struct file_st
{
    struct file_st *parent;
    struct file_st *childrem;
    struct file_st *brother;
    file_type_e type;
    char name[FILE_NAME_SIZE];
    char path[FILE_PATH_SIZE];
    char re_path[FILE_PATH_SIZE];
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
void print_total_size(int size);
void print_files(struct file_st *f);
void print_file_tree(struct file_tree_st *root);
void print_metadata(struct file_tree_st* root);
void get_relative_path(char* path, char* relative_path, char* root_folder);
void get_file_name_from_path(char* path, char* file_name);

#endif //__FILE_HANDLER_H__
