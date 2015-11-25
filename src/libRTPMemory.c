#include "libRTPMemory.h"
#include "libRTPCheckMacros.h"
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

int
check_buffer_size_and_realloc(
    uint8_t** p_p_buffer,
    size_t* p_buffer_size,
    size_t data_size,
    uint8_t** p_p_buffer_current_position,
    size_t size_of_data_going_to_write)
{
    uint8_t* p_temp;
    if((*p_buffer_size) - data_size < size_of_data_going_to_write)
    {
        (*p_buffer_size) = (size_of_data_going_to_write)* 2 + (*p_buffer_size);
        p_temp = realloc((*p_p_buffer), (*p_buffer_size));
        CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(p_temp);
        (*p_p_buffer) = p_temp;
        (*p_p_buffer_current_position) = (*p_p_buffer) + data_size;
    }
    return LIBRTP_OK;
}

