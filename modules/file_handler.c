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

