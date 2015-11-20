#include "libRTPMemory.h"
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG
#endif // _MSC_VER

void* libRTP_malloc(size_t size)
{
    return malloc(size);
}

void* libRTP_calloc(size_t size)
{
    return calloc(1, size);
}

void* libRTP_realloc(void* pointer, size_t new_size)
{
    return realloc(pointer, new_size);
}

void libRTP_free(void* pointer)
{
    // I heared that in some machine free() will crash when freeing NULL pointer
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