#include "libRTPH264PayloadProcessor.h"

void H264_payload_processor(RTP_data* p_RTP_data)
{
    NALU_t base_NALU;
    base_NALU.forbidden_zero_bit = ((NALU_HEADER*)(p_RTP_data->payload_start_position))->F;
    base_NALU.NAL_reference_idc = ((NALU_HEADER*)(p_RTP_data->payload_start_position))->NRI;
    base_NALU.NAL_unit_type = ((NALU_HEADER*)(p_RTP_data->payload_start_position))->TYPE;

    // Single NAL Unit Packet
    if(0 < base_NALU.NAL_unit_type && base_NALU.NAL_unit_type < 24)
    {
        // NAL Unit header and data is good to use
        p_RTP_data->payload_start_position -= 4;
        p_RTP_data->payload_size += 4;
        // add 0x00000001 for framing
        p_RTP_data->payload_start_position[0] = 0x0;
        p_RTP_data->payload_start_position[1] = 0x0;
        p_RTP_data->payload_start_position[2] = 0x0;
        p_RTP_data->payload_start_position[3] = 0x1;
    }
    // Fragmentation Units A (FU-A), decoding order and net transfer order are same
    else if(28 == base_NALU.NAL_unit_type)
    {
        // the last package of FU-A
        if(1 == p_RTP_data->RTP_package_byte_1.little_endian.M)
        {
            p_RTP_data->payload_start_position += 2;
            p_RTP_data->payload_size -= 2;
        }
        // FU-A, but not last package
        else if(0 == p_RTP_data->RTP_package_byte_1.little_endian.M)
        {
            uint8_t NAL_header_temp =
                (((FU_INDICATOR*)(p_RTP_data->payload_start_position))->F << 7) |
                (((FU_INDICATOR*)(p_RTP_data->payload_start_position))->NRI << 5);
            p_RTP_data->payload_start_position += 1;
            p_RTP_data->payload_size -= 1;

            // FU-A and first package, reconstruct NAL Unit header
            if(((FU_HEADER*)(p_RTP_data->payload_start_position))->S == 1)
            {
                NAL_header_temp |= ((FU_HEADER*)(p_RTP_data->payload_start_position))->TYPE;

                *(p_RTP_data->payload_start_position) = NAL_header_temp;

                p_RTP_data->payload_start_position -= 4;
                p_RTP_data->payload_size += 4;

                // 0x00000001 for framing
                p_RTP_data->payload_start_position[0] = 0x0;
                p_RTP_data->payload_start_position[1] = 0x0;
                p_RTP_data->payload_start_position[2] = 0x0;
                p_RTP_data->payload_start_position[3] = 0x1;
            }
            // Fragmentation Units A (FU-A) not first package nor last
            else
            {
                p_RTP_data->payload_start_position += 1;
                p_RTP_data->payload_size -= 1;
            }
        }
    }
}
