#include "libRTPPSPayloadProcessor.h"

void PS_payload_processor(RTP_data* p_RTP_data)
{
    pack_layer pack_layer_temp = {0};
    uint64_t temp_uint64_t = 0;
    uint32_t temp_uint32_t = 0;
    for(size_t i = 0; i < sizeof(uint32_t); i++)
    {
        ((uint8_t*)(&pack_layer_temp.pack_start_code))[i] = p_RTP_data->payload_start_position[sizeof(uint32_t) - i - 1];
    }

    p_RTP_data->payload_start_position += 4;
    p_RTP_data->payload_size -= 4;

    pack_layer_temp.two_bit = p_RTP_data->payload_start_position[0] >> 6;

    temp_uint64_t = *((uint64_t*)p_RTP_data->payload_start_position);
    temp_uint64_t = temp_uint64_t << 2;
    temp_uint64_t = temp_uint64_t >> 22;
    for(size_t i = 0; i < sizeof(uint64_t); i++)
    {
        ((uint8_t*)(&pack_layer_temp.system_clock_reference))[i] = ((uint8_t*)(&temp_uint64_t))[sizeof(uint64_t) - i - 1];
    }

    p_RTP_data->payload_start_position += 5;
    p_RTP_data->payload_size -= 5;

    temp_uint32_t = *((uint32_t*)p_RTP_data->payload_start_position);
    temp_uint32_t = temp_uint32_t << 4;
    temp_uint32_t = temp_uint32_t >> 10;
    for(size_t i = 0; i < sizeof(uint32_t); i++)
    {
        ((uint8_t*)(&pack_layer_temp.program_mux_rate))[i] = ((uint8_t*)(&temp_uint32_t))[sizeof(uint32_t) - i - 1];
    }

    p_RTP_data->payload_start_position += 3;
    p_RTP_data->payload_size -= 3;


}