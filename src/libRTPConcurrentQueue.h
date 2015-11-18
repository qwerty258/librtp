#pragma once
#ifndef _LIBRTP_CONCURRENT_QUEUE_H_
#define _LIBRTP_CONCURRENT_QUEUE_H_

#include <stdbool.h>

typedef void* concurrent_queue_handle;

concurrent_queue_handle get_concurrent_queue(void);

bool concurrent_pushback(concurrent_queue_handle handle, void* element);

void* concurrent_pophead(concurrent_queue_handle handle);

void free_concurrent_queue(concurrent_queue_handle* handle);

#endif // !_LIBRTP_CONCURRENT_QUEUE_H_
