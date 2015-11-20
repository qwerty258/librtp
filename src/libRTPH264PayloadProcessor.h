#pragma once
#ifndef _LIBRTP_H264_PAYLOAD_PROCESSOR_H_
#define _LIBRTP_H264_PAYLOAD_PROCESSOR_H_

#include "libRTPDataDefines.h"

/*

Example of NRI values for coded slices and coded slice
data partitions of primary coded reference pictures

NAL Unit Type | Content of NAL Unit          | NRI(binary)
--------------+------------------------------+------------
1             | non - IDR coded slice        | 10
2             | Coded slice data partition A | 10
3             | Coded slice data partition B | 01
4             | Coded slice data partition C | 01

*/

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*/
typedef struct
{
    uint8_t TYPE : 5;
    uint8_t NRI : 2;
    uint8_t F : 1;
} NALU_HEADER;

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*/
typedef struct
{
    uint8_t TYPE : 5;
    uint8_t NRI : 2;
    uint8_t F : 1;
} FU_INDICATOR;

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|S|E|R|  Type   |
+---------------+
*/
typedef struct
{
    uint8_t TYPE : 5;
    uint8_t R : 1;
    uint8_t E : 1;
    uint8_t S : 1;
} FU_HEADER;

typedef struct
{
    // A value of 0 indicates that the NAL unit
    // type octet and payload should not contain bit errors or other
    // syntax violations.A value of 1 indicates that the NAL unit
    // type octet and payload may contain bit errors or other syntax
    // violations.
    uint8_t  forbidden_zero_bit;
    uint8_t  NAL_reference_idc;   // NALU_PRIORITY_xxxx
    uint8_t  NAL_unit_type;       // NALU_TYPE_xxxx
    uint32_t   startcodeprefix_len; // prefix bytes
    uint32_t   len;                 // including nal header's length, from first 00000001 to next 000000001
    uint32_t   max_size;            // make more nal's length
} NALU_t;


void H264_payload_processor(RTP_data* p_RTP_data);

#endif // !_LIBRTP_H264_PAYLOAD_PROCESSOR_H_
