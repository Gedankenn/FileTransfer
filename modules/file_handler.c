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
void print_files(struct file_st *f, int depth)
{
    if (f == NULL)
        return;

    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    printf("|-- %s\n", f->name);

    // First: recurse into children
    if (f->childrem) {
        print_files(f->childrem, depth + 1);
    }

    // Then: move to next brother
    if (f->brother) {
        print_files(f->brother, depth);
    }
	return;
}

void print_file_tree(struct file_tree_st *root)
{
    printf("------------- Print file tree for -------------\n");
    printf("Path: %s\n", root->root->path);
    printf("Total Size: %d mb\n", (root->total_size / 1024) / 1024);
    printf("Total File count: %d\n", root->files_count);
    printf("Total folder count: %d\n", root->folders_count);

    print_files(root->root, 0);
}

void scan_dir(const char *path, int depth, struct file_st *file, struct file_tree_st *root)
{
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (dp == NULL) 
    {
        perror("opendir");
        return;
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
            printf("  ");
        }
        printf("|-- %s\n", entry->d_name);
        #endif
        // Build new path
		struct file_st *q;
		q = malloc(sizeof(struct file_st));
		init_file_node(q);
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        snprintf(q->path, FILE_PATH_SIZE, "%s", fullpath);
        snprintf(q->name, FILE_NAME_SIZE, "%s", entry->d_name);

        // If it's a directory, recurse
        struct stat statbuf;
        if (stat(fullpath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode))
        {
            q->type = DIRECTORY_E;
            file->childrem = q;
			q->parent = file;
            root->folders_count++;
            scan_dir(fullpath, depth + 1, q, root);
        }
        else
        {
            q->type = FILE_E;
            file->brother = q;
            root->files_count++;
            root->total_size = root->total_size + statbuf.st_size;
			file = q;
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

    root->total_size = 0;
    root->files_count = 0;
    root->folders_count = 0;
    root->root = malloc(sizeof(struct file_st));
    init_file_node(root->root);
    root->root->type = DIRECTORY_E;
    snprintf(root->root->path, FILE_PATH_SIZE, "%s", path);
    snprintf(root->root->name, FILE_NAME_SIZE, "%s", path);
    scan_dir(path, 0, root->root, root);
    
#ifdef DEBUG
    print_file_tree(root);
#endif
    return SUCCESS;
}
