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

typedef void(*payload_processer_function)(RTP_data* p_RTP_data);

typedef struct _payload_processor_context
{
    uint32_t payload_type;
    payload_processer_function p_function;
}payload_processor_context;

payload_processor_context* get_payload_processor_table(void);

#endif // !_LIBRTP_PAYLOAD_PROCESSORS_H_
