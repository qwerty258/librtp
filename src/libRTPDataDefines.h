#pragma once
#ifndef _LIBRTP_DATA_DEFINES_H_
#define _LIBRTP_DATA_DEFINES_H_

#include <stdint.h>

/*

The RTP header has the following format:

|0                 1                   2                   3    |
|0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7|
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|V=2|P|X|  CC   |M|     PT      |       sequence number         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                           timestamp                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           synchronization source (SSRC) identifier            |
+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
|            contributing source (CSRC) identifiers             |
|                             ....                              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

Be careful with the endianness

intel cpu is little endian, but network byte order is big endian

intel cpu: high -> csrc_len:4 -> extension:1-> padding:1 -> version:2 -> low
in memory:
low  -> 4001(memory address) version:2
|       4002(memory address) padding:1
|       4003(memory address) extension:1
high -> 4004(memory address) csrc_len:4

network byte order: high ->version:2 -> padding:1 -> extension:1 -> csrc_len:4-> low
when saved into memroy:
low  -> 4001(memory address) version:2
|       4002(memory address) padding:1
|       4003(memory address) extension:1
high -> 4004(memory address) csrc_len:4

local memory: high -> csrc_len:4 -> extension:1 -> padding:1 -> version:2 -> low

*/

typedef struct
{
    unsigned char CC : 4;   // CC: CSRC count, normally 0 in the absence of RTP mixers
    unsigned char X : 1;    // X: Extension, MUST be zero
    unsigned char P : 1;    // P: Padding bit, Padding MUST NOT be used
    unsigned char V : 2;    // V: Version, 2 bits, MUST be 0x2
}RTP_header_byte_0_little_endian;

typedef struct
{
    unsigned char PT : 7;   // PT: 7 bits, Payload Type, dynamically established
    unsigned char M : 1;    // M: Marker bit
}RTP_header_byte_1_little_endian;

typedef struct
{
    unsigned char V : 2;
    unsigned char P : 1;
    unsigned char X : 1;
    unsigned char CC : 4;
}RTP_header_byte_0_big_endian;

typedef struct
{
    unsigned char M : 1;
    unsigned char PT : 7;
}RTP_header_byte_1_big_endian;

typedef union _RTP_header_byte_0
{
    RTP_header_byte_0_little_endian little_endian;
    RTP_header_byte_0_big_endian    big_endian;
}RTP_header_byte_0;

typedef union _RTP_header_byte_1
{
    RTP_header_byte_1_little_endian little_endian;
    RTP_header_byte_1_big_endian    big_endian;
}RTP_header_byte_1;

#define RTP_DATA_BUFFER_SIZE 2048

typedef struct _RTP_data
{
    uint8_t             data[RTP_DATA_BUFFER_SIZE];
    int32_t             socket_received_size;
    uint32_t            payload_size;
    uint8_t*            payload_start_position;
    RTP_header_byte_0   RTP_package_byte_0;
    RTP_header_byte_1   RTP_package_byte_1;
    uint16_t            sequence_number;                // sequence number: increaced by one for each sent packet
    uint32_t            timestamp;                      // timestamp: 27 MHz for H.264
    uint32_t            synchronization_source;         // synchronization source (SSRC) identifier: chosen randomly
    uint32_t            contributing_source_list[16];   // contributing source (CSRC) identifiers: pointer to the array head
}RTP_data;

#endif // !_LIBRTP_DATA_DEFINES_H_
