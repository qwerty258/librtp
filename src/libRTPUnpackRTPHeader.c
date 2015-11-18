#include "libRTPUnpackRTPHeader.h"
#include <stdio.h>

void unpack_RTP_header(RTP_data* p_RTP_data)
{
    if(NULL == p_RTP_data)
    {
        return;
    }

    uint8_t* current_position = p_RTP_data->data;

    // byte 0:
    p_RTP_data->RTP_package_byte_0 = *((RTP_header_byte_0*)current_position);
    current_position++;

    // byte 1:
    p_RTP_data->RTP_package_byte_1 = *((RTP_header_byte_1*)current_position);
    current_position++;

    union _endian_test
    {
        uint32_t a;
        uint8_t  b;
    }endian_test;
    endian_test.a = 1;

    if(1 == endian_test.b) // little
    {
        // byte 2-3: sequence number
        ((uint8_t*)&(p_RTP_data->sequence_number))[0] = current_position[1];
        ((uint8_t*)&(p_RTP_data->sequence_number))[1] = current_position[0];
        current_position += 2;

        // byte 4-7: timestamp
        ((uint8_t*)&(p_RTP_data->timestamp))[0] = current_position[3];
        ((uint8_t*)&(p_RTP_data->timestamp))[1] = current_position[2];
        ((uint8_t*)&(p_RTP_data->timestamp))[2] = current_position[1];
        ((uint8_t*)&(p_RTP_data->timestamp))[3] = current_position[0];
        current_position += 4;

        // byte 8-11: synchronization source (SSRC) identifier
        ((uint8_t*)&(p_RTP_data->synchronization_source))[0] = current_position[3];
        ((uint8_t*)&(p_RTP_data->synchronization_source))[1] = current_position[2];
        ((uint8_t*)&(p_RTP_data->synchronization_source))[2] = current_position[1];
        ((uint8_t*)&(p_RTP_data->synchronization_source))[3] = current_position[0];
        current_position += 4;

        // contributing source (CSRC) identifiers
        if(0 != p_RTP_data->RTP_package_byte_0.little_endian.CC)
        {
            for(uint8_t i = 0; i < p_RTP_data->RTP_package_byte_0.little_endian.CC; ++i)
            {
                ((uint8_t*)&p_RTP_data->contributing_source_list[i])[0] = current_position[3];
                ((uint8_t*)&p_RTP_data->contributing_source_list[i])[1] = current_position[2];
                ((uint8_t*)&p_RTP_data->contributing_source_list[i])[2] = current_position[1];
                ((uint8_t*)&p_RTP_data->contributing_source_list[i])[3] = current_position[0];
                current_position += 4;
            }
        }

        p_RTP_data->payload_start_position = current_position;
        p_RTP_data->payload_size = p_RTP_data->socket_received_size - (current_position - p_RTP_data->data);
    }
    else if(0 == endian_test.b) // big
    {
        // byte 2-3: sequence number
        p_RTP_data->sequence_number = *(uint16_t*)current_position;
        current_position += 2;

        // byte 4-7: timestamp
        p_RTP_data->timestamp = *(uint32_t*)current_position;
        current_position += 4;

        // byte 8-11: synchronization source (SSRC) identifier
        p_RTP_data->synchronization_source = *(uint32_t*)current_position;
        current_position += 4;

        // contributing source (CSRC) identifiers
        if(0 != p_RTP_data->RTP_package_byte_0.little_endian.CC)
        {
            for(uint8_t i = 0; i < p_RTP_data->RTP_package_byte_0.little_endian.CC; ++i)
            {
                p_RTP_data->contributing_source_list[i] = *(uint32_t*)current_position;
                current_position += 4;
            }
        }

        p_RTP_data->payload_start_position = current_position;
        p_RTP_data->payload_size = p_RTP_data->socket_received_size - (current_position - p_RTP_data->data);
    }
}
