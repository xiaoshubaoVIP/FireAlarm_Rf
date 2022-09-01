#include "debug.h"

char* remove_path(char* path)
{
    char * fileName = path;
    while (*++path)
    {
        if (*path == '\\' || *path == '/')
            fileName = path + 1;
    }

    return fileName;
}
