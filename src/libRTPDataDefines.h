#pragma once
#ifndef _LIBRTP_DATA_DEFINES_H_
#define _LIBRTP_DATA_DEFINES_H_

#include <stdint.h>

typedef struct _raw_socket_data
{
    uint8_t data[2000];
    uint32_t size;
}raw_socket_data;

#endif // !_LIBRTP_DATA_DEFINES_H_
