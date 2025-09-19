#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include <sys/stat.h>
#include <unistd.h>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define MKDIR(path) mkdir(path, 0777)
#endif

void init_file_node(struct file_st *f)
{
    f->parent = NULL;
    f->childrem = NULL;
    f->brother = NULL;
    f->size = 0;
}
void print_files(struct file_st *f)
{
    if (f == NULL)
        return;

    // Print indentation
    int depth = f->depth;
    for (int i = 0; i < depth; i++) {
        printf("   ");
    }

    printf("|--- %s\n", f->name);

    // First: recurse into children
    if (f->childrem) {
        print_files(f->childrem);
    }

    // Then: move to next brother
    if (f->brother) {
        print_files(f->brother);
    }
	return;
}

void print_total_size(int size)
{
    int mul = 0;
    int aux_size = size;
    int ret = 0;
    while(aux_size > 0)
    {
        ret = aux_size;
        aux_size = aux_size / 1024;
        mul++;
    }
    mul -=1;
    printf("Total Size: ");
    switch (mul)
    {
        case 0:
            printf("%d b\n",ret);
            break;
        case 1:
            printf("%d kb\n",ret);
            break;
        case 2:
            printf("%d mb\n",ret);
            break;
        case 3:
            printf("%d Gb\n",ret);
            break;
        case 4:
            printf("%d Pb\n",ret);
            break;
    }
}

void print_file_tree(struct file_tree_st *root)
{
    printf("------------- Print file tree for -------------\n");
    printf("Path: %s\n", root->root->path);
    print_total_size(root->total_size);
    printf("Total File count: %d\n", root->files_count);
    printf("Total folder count: %d\n", root->folders_count);

    print_files(root->root);
}

int scan_dir(const char *path, int depth, struct file_st *file, struct file_tree_st *root)
{
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) 
    {
        perror("opendir");
        return ERROR;
    }

    while ((entry = readdir(dp)))
    {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
        {
            continue;
        }

        #ifdef DEBUG
        // Indentation for hierarchy
        for (int i = 0; i < depth; i++)
        {
            printf("    ");
        }
        printf("|---- %s\n", entry->d_name);
        #endif
        // Build new path
		struct file_st *q;
		q = malloc(sizeof(struct file_st));
		init_file_node(q);
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        snprintf(q->path, FILE_PATH_SIZE, "%s", fullpath);
        snprintf(q->name, FILE_NAME_SIZE, "%s", entry->d_name);


        struct stat statbuf;
        if( stat(fullpath, &statbuf) != 0)
        {
            continue;
        }
        q->type = FILE_E;
        q->depth = depth;
        q->parent = file;
        root->total_size += statbuf.st_size;

        // attach q as child of file
        if (file->childrem == NULL)
        {
            file->childrem = q;
        }
        else
        {
            struct file_st *tmp = file->childrem;
            while (tmp->brother != NULL) {
                tmp = tmp->brother;
            }
            tmp->brother = q;
        }

        // If it's a directory, recurse
        if (S_ISDIR(statbuf.st_mode))
        {
            root->folders_count++;
            scan_dir(fullpath, depth + 1, q, root);
        }
        else
        {
            root->files_count++;
            q->size = statbuf.st_size;
            file = q;
        }
    }

    closedir(dp);
    return SUCCESS;
}

void get_file_name_from_path(char* path, char* file_name)
{
    char c;
    c = path[0];
    int pos = 0;
    int p_pos = 0;
    int len = strlen(path);
    while (p_pos <= len)
    {
        if (c == '/')
        {
            pos = 0;
        }
        else 
        {
            file_name[pos++] = c;
        }
        c = path[p_pos++];
    }
    file_name[pos] = '\0';
}

int get_file_bin(struct file_st* file, unsigned char* bin_file)
{
    FILE*  fp;
    int read_count = 0;

    fp = fopen(file->path,"rb");
    if (fp == NULL)
    {
        perror("Error opening file\n");
        return ERROR;
    }
    
    bin_file = (unsigned char*)malloc(file->size);
    read_count = fread(bin_file,1,file->size, fp);
    if(read_count != file->size)
    {
        perror("Error reading file\n");
        return ERROR;
    }
    return SUCCESS;
}

bool folderExist(char* path)
{
	struct stat info;
	if(stat(path, &info) != 0)
	{
		return 0;
	}
	if(S_ISDIR(info.st_mode))
	{
		return 1;
	}
	return 0;
}

bool fileExist(char* path)
{
	struct stat info;
	if(stat(path, &info) != 0)
	{
		return 0;
	}
	if(!S_ISDIR(info.st_mode))
	{
		return 1;
	}
	return 0;
}

bool create_dir(char* path)
{
    if(!folderExist(path))
    {
        if (MKDIR(path) == -1)
        {
            perror("Failed to create folder\n");
            return false;
        }
    }
    return true;
}

bool create_file(char* path)
{
    FILE* fp;
    if (!fileExist(path))
    {
        fp = fopen(path, "w");
        if( fp == NULL)
        {
            return false;
        }
    }
    return true;
}

int read_dir(char *path, struct file_tree_st* root)
{
    char file_name[FILE_NAME_SIZE];
    int ret = 0;


    root->total_size = 0;
    root->files_count = 0;
    root->folders_count = 0;
    root->root = malloc(sizeof(struct file_st));
    init_file_node(root->root);
    root->root->type = DIRECTORY_E;
    get_file_name_from_path(path, file_name);

#ifdef DEBUG
    printf("--------------- Debug ------------------\n");
    printf("cwd: %s\n", path);
    printf("Contents of directory: %s\n", file_name);
    printf("----------------------------------------\n");
#endif

    snprintf(root->root->path, FILE_PATH_SIZE, "%s", path);
    snprintf(root->root->name, FILE_NAME_SIZE, "%s", file_name);
    init_file_node(root->root);
    ret = scan_dir(path, 1, root->root, root);

#ifdef DEBUG
    print_file_tree(root);
#endif
    return ret;
}
