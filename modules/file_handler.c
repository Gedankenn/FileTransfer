#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include "color.h"
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
    for (int i = 0; i < depth && depth < 12; i++)
    {
        printf("   ");
    }

    printf("|--- %s\n", f->name);

    // First: recurse into children
    if (f->childrem)
    {
        print_files(f->childrem);
    }

    // Then: move to next brother
    if (f->brother)
    {
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

void get_file_name_from_path(char* path, char* file_name)
{
    char c;
    int pos = 0;
    int p_pos = 0;
    int len = strlen(path);
    while (p_pos <= len)
    {
        c = path[p_pos++];
        if (c == '/')
        {
            pos = 0;
        }
        else 
        {
            file_name[pos++] = c;
        }
    }
    file_name[pos] = '\0';
}

void get_relative_path(const char* path, const char* relative_path, char* root_folder)
{
    char aux[FILE_PATH_SIZE];
    aux[0] = '\0';
    int pos = 0;
    int pos2 = 0;
    int path_len = strlen(path);

    while (strcmp(aux, root_folder) != 0 && pos2 < path_len)
    {
        pos = 0;
        while (path[pos2] != PATH_SEP && pos2 < path_len)
        {
            aux[pos] = path[pos2];
            pos++;
            pos2++;
        }
        aux[pos] = '\0';
        pos2++;
    }
    snprintf((char*)relative_path, FILE_PATH_SIZE, "%s", &path[pos2-pos-1]);
}

void print_metadata(struct file_tree_st* root)
{
    printf("%s------------------ root file metadata -------------------\n",ANSI_MAGENTA);
    if(root->root != NULL)
    {
        printf("Path: %s\n", root->root->path);
    }
    print_total_size(root->total_size);
    printf("Files count: %d\n", root->files_count);
    printf("Folders count: %d\n", root->folders_count);
    printf("----------------------------------------------------------\n%s", ANSI_RESET);
}

void print_file_tree(struct file_tree_st *root)
{
    printf("%s\n",ANSI_YELLOW);
    print_metadata(root);
    print_files(root->root);
    printf("-----------------------------------------------------\n");
    printf("%s\n",ANSI_RESET);
}

int scan_dir(const char *path, int depth, struct file_st *parent, struct file_tree_st *root)
{
    if (strcmp(path, ".") == 0)
    {
        return PATH_ERROR;
    }

    DIR *dp = opendir(path);
    if (!dp)
    {
        perror("opendir");
        return ERROR;
    }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL)
    {
        // Skip "." and ".." and hidden files
        if (entry->d_name[0] == '.' )
        {
            continue;
        }

        // Build full path (avoid double '/')
        char fullpath[PATH_MAX];
        if (path[0] && path[strlen(path)-1] == PATH_SEP)
            snprintf(fullpath, sizeof fullpath, "%s%s", path, entry->d_name);
        else
            snprintf(fullpath, sizeof fullpath, "%s%c%s", path, PATH_SEP, entry->d_name);

        // Stat before allocating
        struct stat st;
        if (stat(fullpath, &st) != 0)
        {
            // could log perror("stat") here
            continue;
        }

        // Now allocate the node
        struct file_st *q = malloc(sizeof *q);
        if (!q) { closedir(dp); return ERROR; }
        init_file_node(q);

        // Fill fields
        snprintf(q->path, FILE_PATH_SIZE, "%s", fullpath);
        snprintf(q->name, FILE_NAME_SIZE, "%s", entry->d_name);
        q->depth  = depth + 1;
        q->parent = parent;

        // Compute relative path of THIS entry against the tree root
        // (don’t pass a shared buffer around)
        get_relative_path(q->path, q->re_path, root->root->name);

        // Type & counters
        if (S_ISDIR(st.st_mode))
        {
            q->type = DIRECTORY_E;
            root->folders_count++;
        } else {
            q->type = FILE_E;
            q->size = st.st_size;
            root->files_count++;
            root->total_size += st.st_size; // count only files
        }

        // Attach as last child of 'parent'
        if (parent->childrem == NULL)
        {
            parent->childrem = q;
        } else {
            struct file_st *tmp = parent->childrem;
            while (tmp->brother) tmp = tmp->brother;
            tmp->brother = q;
        }

        // Recurse into directories (do NOT reassign 'parent' for siblings)
        if (S_ISDIR(st.st_mode))
        {
            scan_dir(fullpath, depth + 1, q, root);
        }
    }

    closedir(dp);
    return SUCCESS;
}

int get_file_bin(struct file_st* file, unsigned char* bin_file)
{
    FILE*  fp;
    int read_count = 0;
    
    fp = fopen(file->path,"rb");
    if (fp == NULL)
    {
        printf("%sError opening file\n%s", ANSI_RED, ANSI_RESET);
        return ERROR;
    }
    
    bin_file = (unsigned char*)malloc(file->size);
    read_count = fread(bin_file,1,file->size - 1, fp);
    if(read_count != file->size)
    {
        printf("%sError reading file\n%s", ANSI_RED, ANSI_RESET);
        printf("File size: %d\nFile read: %d\n", file->size, read_count);
        printf("File path: %s\n",file->path);
        fclose(fp);
        return ERROR;
    }
    fclose(fp);
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
    snprintf(root->root->re_path, FILE_PATH_SIZE, "%s", file_name);
    root->root->depth = 0;
    init_file_node(root->root);
    ret = scan_dir(path, 1, root->root, root);

#ifdef DEBUG
    print_file_tree(root);
#endif
    free(root->root);
    return ret;
}
