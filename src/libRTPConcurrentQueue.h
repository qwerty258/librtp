#pragma once
#ifndef _LIBRTP_CONCURRENT_QUEUE_H_
#define _LIBRTP_CONCURRENT_QUEUE_H_

#include <stdbool.h>

typedef void* concurrent_queue_handle;

concurrent_queue_handle concurrent_queue_get_handle(void);

bool concurrent_queue_pushback(concurrent_queue_handle handle, void* element);

void* concurrent_queue_pophead(concurrent_queue_handle handle);

void concurrent_queue_free(concurrent_queue_handle* handle);

#endif // !_LIBRTP_CONCURRENT_QUEUE_H_
