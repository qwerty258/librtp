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
    unsigned char TYPE : 5;
    unsigned char NRI : 2;
    unsigned char F : 1;
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
    unsigned char TYPE : 5;
    unsigned char NRI : 2;
    unsigned char F : 1;
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
    unsigned char TYPE : 5;
    unsigned char R : 1;
    unsigned char E : 1;
    unsigned char S : 1;
} FU_HEADER;

typedef struct
{
    // A value of 0 indicates that the NAL unit
    // type octet and payload should not contain bit errors or other
    // syntax violations.A value of 1 indicates that the NAL unit
    // type octet and payload may contain bit errors or other syntax
    // violations.
    unsigned char  forbidden_zero_bit;
    unsigned char  NAL_reference_idc;   // NALU_PRIORITY_xxxx
    unsigned char  NAL_unit_type;       // NALU_TYPE_xxxx
    unsigned int   startcodeprefix_len; // prefix bytes
    unsigned int   len;                 // including nal header's length, from first 00000001 to next 000000001
    unsigned int   max_size;            // make more nal's length
} NALU_t;


void H264_payload_processor(RTP_data* p_RTP_data);

#endif // !_LIBRTP_H264_PAYLOAD_PROCESSOR_H_
