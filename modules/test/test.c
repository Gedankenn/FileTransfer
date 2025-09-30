#include <stdio.h>
#include "../file_handler.h"


void test_get_relative_path(void)
{
    char relative_path[FILE_PATH_SIZE];
    char path[FILE_PATH_SIZE];
    char root_folder[FILE_NAME_SIZE];

    snprintf(path, FILE_PATH_SIZE, "/home/sabo/github/test");
    get_file_name_from_path(path, root_folder);

    get_relative_path(path, relative_path, root_folder);

    printf("path: %s\n",path);
    printf("root_folder: %s\n",root_folder);
    printf("relative path: %s\n",relative_path);

}

int main(void)
{
    test_get_relative_path();
    return 0;
}
