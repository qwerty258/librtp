#pragma once
#ifndef _LIBRTP_MEMORY_H_
#define _LIBRTP_MEMORY_H_

#include <stdint.h>
#include "libRTPErrorDefine.h"

void* libRTP_malloc(size_t size);

void* libRTP_calloc(size_t size);

void* libRTP_realloc(void* pointer, size_t new_size);

void libRTP_free(void* pointer);

char* libRTP_strdup(char* string);

int
check_buffer_size_and_realloc(
    uint8_t** p_p_buffer,                   // pointer to the pointer of the buffer
    size_t* p_buffer_size,                  // current buffer size
    size_t data_size,                       // already written data size
    uint8_t** p_p_buffer_current_position,  // pointer to the pointer of byte behind the last data byte
    size_t size_of_data_going_to_write);    // size of data going to write

#endif // !_LIBRTP_MEMORY_H_
