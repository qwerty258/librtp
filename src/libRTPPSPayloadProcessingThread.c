#include "libRTPSessions.h"
#include "libRTPMemory.h"
#include "libRTPCheckMacros.h"
#include "libRTPPSPayloadProcessingThread.h"

#define CHECK_PS_BUFFER_SIZE(size)                                          \
if(PS_buffer_size - (p_PS_buffer_current_position - p_PS_buffer) < (size))  \
{                                                                           \
    PS_buffer_size = (size) * 2 + PS_buffer_size;                           \
    p_temp = libRTP_realloc(p_PS_buffer, PS_buffer_size);                   \
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(p_temp);                        \
    p_PS_buffer = p_temp;                                                   \
    p_PS_buffer_current_position = p_PS_buffer + PS_buffer_size;            \
}

uint32_t WINAPI PS_payload_processing_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    uint8_t* p_PS_buffer = libRTP_calloc(USHRT_MAX);
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(p_PS_buffer);
    uint8_t* p_temp = NULL;
    uint8_t* p_PS_buffer_current_position = NULL;
    size_t PS_data_size = 0;
    size_t PS_buffer_size = USHRT_MAX;

    RTP_data* p_RTP_data = NULL;

    uint32_t previous_timestamp = 0;
    uint64_t previous_frame_ID = 0;
    uint64_t current_frame_ID = 0;
    uint32_t previous_number_of_lost_package = 0;
    uint32_t current_number_of_lost_package = 0;
    uint16_t previous_sequence_number = 0;

    PS_data_size = 0;
    p_PS_buffer_current_position = p_PS_buffer;
    while(p_RTP_session_context->session_started)
    {
        p_RTP_data = concurrent_queue_pophead(p_RTP_session_context->concurrent_queue_handle_for_payload);
        if(NULL == p_RTP_data)
        {
            Sleep(10);
        }
        // form PS data
        else
        {
            CHECK_PS_BUFFER_SIZE(p_RTP_data->payload_size);
            memcpy(p_PS_buffer_current_position, p_RTP_data->payload_start_position, p_RTP_data->payload_size);
            p_PS_buffer_current_position += p_RTP_data->payload_size;
            PS_data_size += p_RTP_data->payload_size;

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
                    p_PS_buffer,
                    PS_data_size,
                    0,
                    0);
                p_PS_buffer_current_position = p_PS_buffer;
                PS_data_size = 0;
            }
        }
    }

    libRTP_free(p_PS_buffer);

    return 0;
}