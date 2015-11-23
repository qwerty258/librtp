#pragma once
#ifndef _LIBRTP_PS_PAYLOAD_PROCESSOR_H_
#define _LIBRTP_PS_PAYLOAD_PROCESSOR_H_

#include "libRTPDataDefines.h"

// ISO/IEC 13818-1 : 2000 program stream(PS)

#define PROGRAM_STREAM_MAP______    0xBC
#define PADDING_STREAM__________    0xBE
#define ITU_T_REC_H_222_1_TYPE_E    0xF8
#define PRIVATE_STREAM_2________    0xBF
#define ECM_____________________    0xF0
#define EMM_____________________    0xF1
#define PROGRAM_STREAM_DIRECTORY    0xFF
#define DSMCC_STREAM____________    0XF2

typedef struct _system_clock_reference
{
    // The first part,
    // system_clock_reference_base, is a 33 - bit
    // field whose value is given by SCR_base(i) as
    // given in equation 2 - 19.
    uint64_t system_clock_reference_base;
    // The second part, system_clock_reference_extension,
    // is a 9 - bit field whose value
    // is given by SCR_ext(i), as given in equation 2 - 20.
    // The SCR indicates the intended time of arrival of the byte
    // containing the last bit of the system_clock_reference_base
    // at the input of the program target decoder.
    uint16_t system_clock_reference_extension;
}system_clock_reference;

typedef struct _pack_layer
{
    // The pack_start_code is the bit string
    // '0000 0000 0000 0000 0000 0001 1011 1010' (0x000001BA).
    // It identifies the beginning of a pack.
    uint32_t pack_start_code;
    // must be '01'
    uint8_t two_bit;
    // The system clock reference (SCR) is a 42-bit
    // field coded in two parts.
    system_clock_reference system_clock_reference;
    // This is a 22-bit integer specifying the rate
    // at which the P-STD receives the Program Stream
    // during the pack in which it is included.
    uint32_t program_mux_rate;
    // no use, ignore
    uint8_t five_bit;
    // A 3-bit integer specifying the number of
    // stuffing bytes which follow this field.
    uint8_t pack_stuffing_length;
}pack_layer;

typedef struct _system_header
{
    // This 16 - bit field indicates the length in bytes of
    // the system header following the header_length field.
    // Future extensions of this Specification may extend the system header.
    uint16_t header_length;
    // The system_header_start_code is the bit string
    // '0000 0000 0000 0000 0000 0001 1011 1011' (0x000001BB).
    // It identifies the beginning of a system header
    uint32_t system_header_start_code;
    // A 22-bit field. The rate_bound is an integer value
    // greater than or equal to the maximum value of
    // the program_mux_rate field coded in any pack of the Program Stream.
    // It may be used by a decoder to assess whether it is
    // capable of decoding the entire stream.
    uint32_t rate_bound;
    // A 6-bit field. The audio_bound is an integer
    // in the inclusive range from 0 to 32 and
    // is set to a value greater than or equal to the maximum number
    // of ISO / IEC 13818 - 3 and ISO / IEC 11172 - 3 audio streams
    // in the Program Stream for which the decoding processes are simultaneously active.
    // For the purpose of this subclause, the decoding process of
    // an ISO / IEC 13818 - 3 or ISO / IEC 11172 - 3 audio stream is
    // active if the STD buffer is not empty or if a Presentation Unit is
    // being presented in the P - STD model.
    uint8_t audio_bound;
    uint8_t fixed_flag;
    uint8_t CSPS_flag;
    uint8_t system_audio_lock_flag;
    uint8_t system_video_lock_flag;
    uint8_t video_bound;
    uint8_t packet_rate_restriction_flag;
}system_header;

typedef struct _PES_packet
{
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;
}PES_packet;

void PS_payload_processor(RTP_data* p_RTP_data);

#endif // !_LIBRTP_PS_PAYLOAD_PROCESSOR_H_
