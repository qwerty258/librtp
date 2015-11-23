#pragma once
#ifndef _LIBRTP_PAYLOAD_PROCESSORS_H_
#define _LIBRTP_PAYLOAD_PROCESSORS_H_

#include "libRTPDataDefines.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)  \
((uint32_t)(uint8_t)(ch0) |             \
((uint32_t)(uint8_t)(ch1) << 8) |       \
((uint32_t)(uint8_t)(ch2) << 16) |      \
((uint32_t)(uint8_t)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

typedef uint32_t(*function_payload_processing_thread)(void* parameter);

typedef struct _payload_processor_context
{
    uint32_t payload_type;
    function_payload_processing_thread p_function_payload_processing_thread;
}payload_processor_context;

payload_processor_context* get_payload_processor_table(void);

#endif // !_LIBRTP_PAYLOAD_PROCESSORS_H_
