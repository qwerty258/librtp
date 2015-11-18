#pragma once
#ifndef _LIBRTP_MEMORY_H_
#define _LIBRTP_MEMORY_H_

#include <crtdefs.h>
#include "libRTPErrorDefine.h"

#define CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(pointer)    \
if(NULL == (pointer))                                       \
{                                                           \
    return LIBRTP_MEMORY_ERROR;                             \
}                                                           \

#define CHECK_NULL_PARAMETER_AND_RETURN(pointer)    \
if(NULL == (pointer))                               \
{                                                   \
    return LIBRTP_BAD_PARAMETER;                    \
}

void* libRTP_malloc(size_t size);

void* libRTP_calloc(size_t size);

// I heared that in some machine free() will crash when freeing NULL pointer

void libRTP_free(void* pointer);

char* libRTP_strdup(char* string);

#endif // !_LIBRTP_MEMORY_H_
