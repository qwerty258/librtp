#include "libRTPMemory.h"
#include <stdlib.h>

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
