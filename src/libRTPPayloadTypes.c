#include "libRTPPayloadTypes.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)  \
((uint32_t)(uint8_t)(ch0) |             \
((uint32_t)(uint8_t)(ch1) << 8) |       \
((uint32_t)(uint8_t)(ch2) << 16) |      \
((uint32_t)(uint8_t)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

LIBRTP_API uint32_t payload_type_H264 = MAKEFOURCC('H', '2', '6', '4');
LIBRTP_API uint32_t payload_type_PS = MAKEFOURCC(' ', ' ', 'P', 'S');
