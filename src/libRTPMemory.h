#pragma once
#ifndef _LIBRTP_MEMORY_H_
#define _LIBRTP_MEMORY_H_

#include <crtdefs.h>
#include "libRTPErrorDefine.h"

void* libRTP_malloc(size_t size);

void* libRTP_calloc(size_t size);

// I heared that in some machine free() will crash when freeing NULL pointer

void libRTP_free(void* pointer);

char* libRTP_strdup(char* string);

#endif // !_LIBRTP_MEMORY_H_
