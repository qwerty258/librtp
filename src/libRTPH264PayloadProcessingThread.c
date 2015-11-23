#include "libRTPH264PayloadProcessingThread.h"
#include "libRTPSessions.h"
#include "libRTPMemory.h"
#include "libRTPCheckMacros.h"

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

#define CHECK_H264_BUFFER_SIZE(size)                                                \
if(H264_buffer_size - (p_H264_buffer_current_position - p_H264_buffer) < (size))    \
{                                                                                   \
    H264_buffer_size = (size) * 2 + H264_buffer_size;                               \
    p_temp = libRTP_realloc(p_H264_buffer, H264_buffer_size);                       \
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(p_temp);                                \
    p_H264_buffer = p_temp;                                                         \
    p_H264_buffer_current_position = p_H264_buffer + H264_data_size;                \
}

uint32_t WINAPI H264_payload_processing_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    uint8_t* p_H264_buffer = libRTP_calloc(USHRT_MAX);
    uint8_t* p_temp = NULL;
    uint8_t* p_H264_buffer_current_position = NULL;
    uint32_t H264_data_size = 0;
    uint32_t H264_buffer_size = USHRT_MAX;
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(p_H264_buffer);
    RTP_data* p_RTP_data = NULL;
    NALU_t base_NALU = {0};

    uint32_t previous_timestamp = 0;
    uint64_t previous_frame_ID = 0;
    uint64_t current_frame_ID = 0;
    uint32_t previous_number_of_lost_package = 0;
    uint32_t current_number_of_lost_package = 0;
    uint16_t previous_sequence_number = 0;

    H264_data_size = 0;
    p_H264_buffer_current_position = p_H264_buffer;
    while(p_RTP_session_context->session_started)
    {
        p_RTP_data = concurrent_queue_pophead(p_RTP_session_context->concurrent_queue_handle_for_payload);
        if(NULL == p_RTP_data)
        {
            Sleep(10);
        }
        // form H264 data
        else
        {
            base_NALU.forbidden_zero_bit = ((NALU_HEADER*)(p_RTP_data->payload_start_position))->F;
            base_NALU.NAL_reference_idc = ((NALU_HEADER*)(p_RTP_data->payload_start_position))->NRI;
            base_NALU.NAL_unit_type = ((NALU_HEADER*)(p_RTP_data->payload_start_position))->TYPE;

            // Single NAL Unit Packet
            if(0 < base_NALU.NAL_unit_type && base_NALU.NAL_unit_type < 24)
            {
                // 0x00000001 for framing
                CHECK_H264_BUFFER_SIZE(4);
                p_H264_buffer_current_position[0] = 0x00;
                p_H264_buffer_current_position[1] = 0x00;
                p_H264_buffer_current_position[2] = 0x00;
                p_H264_buffer_current_position[3] = 0x01;
                p_H264_buffer_current_position += 4;
                H264_data_size += 4;
                // directly copy header and data to H264 data buffer
                CHECK_H264_BUFFER_SIZE(p_RTP_data->payload_size);
                memcpy(p_H264_buffer_current_position, p_RTP_data->payload_start_position, p_RTP_data->payload_size);
                p_H264_buffer_current_position += p_RTP_data->payload_size;
                H264_data_size += p_RTP_data->payload_size;
            }
            // Fragmentation Units A (FU-A), decoding order and net transfer order are same
            else if(28 == base_NALU.NAL_unit_type)
            {
                // the last package of FU-A, directly copy data to H264 data buffer
                if(1 == p_RTP_data->RTP_package_byte_1.little_endian.M)
                {
                    p_RTP_data->payload_start_position += 2;
                    p_RTP_data->payload_size -= 2;
                    CHECK_H264_BUFFER_SIZE(p_RTP_data->payload_size);
                    memcpy(p_H264_buffer_current_position, p_RTP_data->payload_start_position, p_RTP_data->payload_size);
                    p_H264_buffer_current_position += p_RTP_data->payload_size;
                    H264_data_size += p_RTP_data->payload_size;
                }
                // FU-A, but not last package
                else if(0 == p_RTP_data->RTP_package_byte_1.little_endian.M)
                {
                    uint8_t NAL_header_temp =
                        (((FU_INDICATOR*)(p_RTP_data->payload_start_position))->F << 7) |
                        (((FU_INDICATOR*)(p_RTP_data->payload_start_position))->NRI << 5);
                    p_RTP_data->payload_start_position += 1;
                    p_RTP_data->payload_size -= 1;
                    // FU-A and first package, reconstruct NAL Unit header, then put header and data into unpack result
                    if(((FU_HEADER*)(p_RTP_data->payload_start_position))->S == 1)
                    {
                        // 0x00000001 for framing
                        CHECK_H264_BUFFER_SIZE(4);
                        p_H264_buffer_current_position[0] = 0x00;
                        p_H264_buffer_current_position[1] = 0x00;
                        p_H264_buffer_current_position[2] = 0x00;
                        p_H264_buffer_current_position[3] = 0x01;
                        p_H264_buffer_current_position += 4;
                        H264_data_size += 4;

                        NAL_header_temp |= ((FU_HEADER*)(p_RTP_data->payload_start_position))->TYPE;
                        CHECK_H264_BUFFER_SIZE(1);
                        p_H264_buffer_current_position[0] = NAL_header_temp;
                        p_H264_buffer_current_position += 1;
                        H264_data_size += 1;

                        p_RTP_data->payload_start_position += 1;
                        p_RTP_data->payload_size -= 1;

                        CHECK_H264_BUFFER_SIZE(p_RTP_data->payload_size);
                        memcpy(p_H264_buffer_current_position, p_RTP_data->payload_start_position, p_RTP_data->payload_size);
                        p_H264_buffer_current_position += p_RTP_data->payload_size;
                        H264_data_size += p_RTP_data->payload_size;
                    }
                    // Fragmentation Units A (FU-A) not first package nor last, directly copy data into unpack result
                    else
                    {
                        p_RTP_data->payload_start_position += 1;
                        p_RTP_data->payload_size -= 1;

                        CHECK_H264_BUFFER_SIZE(p_RTP_data->payload_size);
                        memcpy(p_H264_buffer_current_position, p_RTP_data->payload_start_position, p_RTP_data->payload_size);
                        p_H264_buffer_current_position += p_RTP_data->payload_size;
                        H264_data_size += p_RTP_data->payload_size;
                    }
                }
            }

            // trace lost package
            // when it is a new frame
            if(p_RTP_data->timestamp != previous_timestamp)
            {
                // save previous frame ID and number of lost package
                previous_frame_ID = current_frame_ID;
                previous_number_of_lost_package = current_number_of_lost_package;
                // great change in sequence number, lost frame, not packages
                if(p_RTP_data->sequence_number - previous_sequence_number > 20)
                {
                    current_frame_ID += ((p_RTP_data->sequence_number - previous_sequence_number) / 6);
                }
                // frame ID plus 1
                else
                {
                    ++current_frame_ID;
                }
                // set number of lost package back to 0
                current_number_of_lost_package = 0;
                // set timestamp and sequence number for new check round
                previous_timestamp = p_RTP_data->timestamp;
                previous_sequence_number = p_RTP_data->sequence_number;
            }
            // the same frame
            else
            {
                // if there is lost package, calculate how many lost
                current_number_of_lost_package += (p_RTP_data->sequence_number - previous_sequence_number - 1);
                previous_sequence_number = p_RTP_data->sequence_number;
            }

            if(1 == p_RTP_data->RTP_package_byte_1.little_endian.M)
            {
                p_RTP_session_context->p_function_give_out_payload(
                    p_RTP_session_context->this_session_handle,
                    p_H264_buffer,
                    H264_data_size,
                    0,
                    0);
                p_H264_buffer_current_position = p_H264_buffer;
                H264_data_size = 0;
            }
        }
    }

    libRTP_free(p_H264_buffer);

    return 0;
}