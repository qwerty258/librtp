#include "libRTPMemory.h"
#include <stdlib.h>
#include <string.h>

void* libRTP_malloc(size_t size)
{
    void* pointer = malloc(size);
    return pointer;
}

void* libRTP_calloc(size_t size)
{
    void* pointer = calloc(1, size);
    return pointer;
}

void libRTP_free(void* pointer)
{
    if(NULL != pointer)
    {
        free(pointer);
    }
}

char* libRTP_strdup(char* string)
{
#ifdef _WIN32
    return _strdup(string);
#else
    return strdup(string);
#endif // _WIN32
}