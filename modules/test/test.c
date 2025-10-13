#include <stdio.h>
#include "../file_handler.h"
#include "string.h"


int test_get_relative_path(void)
{
    int success = 0;
    int fails = 0;
    char relative_path[FILE_PATH_SIZE];
    char path[FILE_PATH_SIZE];
    char root_folder[FILE_NAME_SIZE];

    snprintf(path, FILE_PATH_SIZE, "/home/sabo/github/test");
    get_file_name_from_path(path, root_folder);

    get_relative_path(path, relative_path, root_folder);

    if (strcmp(relative_path, root_folder) == 0)
    {
        success++;
    }
    else 
    {
        fails++;
    }

    snprintf(path, FILE_PATH_SIZE, "/home/sabo/github/test/test2");
    get_relative_path(path, relative_path, root_folder);

    if (strcmp(relative_path, "test/test2") == 0)
    {
        success++;
    }
    else 
    {
        fails++;
    }
    
    return success - fails;

}

int main(void)
{
    int ret;
    ret = test_get_relative_path();
    if (ret < 0)
    {
        printf("Testes falharam\n");
    }
    else 
    {
        printf("Testes completados com successo: %d\n", ret);
    }
    return 0;
}
