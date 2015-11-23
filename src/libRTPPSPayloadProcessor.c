#include "libRTPPSPayloadProcessor.h"

void PS_payload_processor(RTP_data* p_RTP_data)
{
    pack_layer pack_layer_temp = {0};
    system_header system_header_temp = {0};
    PES_packet PES_packet_temp = {0};
    uint32_t uint32_temp = 0;
    uint8_t stream_id = 0;
    for(size_t i = 0; i < sizeof(uint32_t); i++)
    {
        ((uint8_t*)(&pack_layer_temp.pack_start_code))[i] = p_RTP_data->payload_start_position[sizeof(uint32_t) - i - 1];
    }

    if(0x000001BA != pack_layer_temp.pack_start_code)
    {
        return;
    }

    p_RTP_data->payload_start_position += sizeof(uint32_t);
    p_RTP_data->payload_size -= sizeof(uint32_t);

    // to do: parse all the unaligned stuff

    // skip all the unaligned stuff
    p_RTP_data->payload_start_position += 10;
    p_RTP_data->payload_size -= 10;

    pack_layer_temp.pack_stuffing_length = *(p_RTP_data->payload_start_position - 1);
    pack_layer_temp.pack_stuffing_length = pack_layer_temp.pack_stuffing_length << 5;
    pack_layer_temp.pack_stuffing_length = pack_layer_temp.pack_stuffing_length >> 5;

    // skip pack_stuffing_length
    p_RTP_data->payload_start_position += pack_layer_temp.pack_stuffing_length;
    p_RTP_data->payload_size -= pack_layer_temp.pack_stuffing_length;

    for(size_t i = 0; i < sizeof(uint32_t); i++)
    {
        ((uint8_t*)(&system_header_temp.system_header_start_code))[i] = p_RTP_data->payload_start_position[sizeof(uint32_t) - i - 1];
    }

    if(0x000001BB != system_header_temp.system_header_start_code)
    {
        return;
    }

    p_RTP_data->payload_start_position += sizeof(uint32_t);
    p_RTP_data->payload_size -= sizeof(uint32_t);

    for(size_t i = 0; i < sizeof(uint16_t); i++)
    {
        ((uint8_t*)(&system_header_temp.header_length))[i] = p_RTP_data->payload_start_position[sizeof(uint16_t) - i - 1];
    }

    p_RTP_data->payload_start_position += sizeof(uint16_t);
    p_RTP_data->payload_size -= sizeof(uint16_t);

    uint32_temp = *(uint32_t*)(p_RTP_data->payload_start_position);
    system_header_temp.rate_bound = uint32_temp & 0x7FFFFE00 >> 9;
    system_header_temp.audio_bound = uint32_temp & 0x000000FC >> 2;
    system_header_temp.fixed_flag = uint32_temp & 0x00000002 >> 1;
    system_header_temp.CSPS_flag = uint32_temp & 0x00000001;
    uint32_temp = system_header_temp.rate_bound;
    for(size_t i = 0; i < sizeof(uint32_t); i++)
    {
        ((uint8_t*)(&system_header_temp.rate_bound))[i] = ((uint8_t*)(&uint32_temp))[sizeof(uint32_t) - i - 1];
    }

    p_RTP_data->payload_start_position += sizeof(uint32_t);
    p_RTP_data->payload_size -= sizeof(uint32_t);

    system_header_temp.system_audio_lock_flag = *(p_RTP_data->payload_start_position) >> 7;
    system_header_temp.system_video_lock_flag = *(p_RTP_data->payload_start_position) << 1 >> 7;
    system_header_temp.video_bound = *(p_RTP_data->payload_start_position) << 3 >> 3;

    p_RTP_data->payload_start_position++;
    p_RTP_data->payload_size--;

    system_header_temp.packet_rate_restriction_flag = *(p_RTP_data->payload_start_position) >> 7;

    p_RTP_data->payload_start_position++;
    p_RTP_data->payload_size--;

    while(0 != *(p_RTP_data->payload_start_position) >> 7)
    {
        stream_id = *(p_RTP_data->payload_start_position);
        p_RTP_data->payload_start_position += 3;
        p_RTP_data->payload_size -= 3;
    }

    while(true)
    {
        for(size_t i = 0; i < sizeof(uint32_t); i++)
        {
            ((uint8_t*)(&PES_packet_temp.packet_start_code_prefix))[i] = p_RTP_data->payload_start_position[sizeof(uint32_t) - i - 1];
        }
        PES_packet_temp.packet_start_code_prefix = PES_packet_temp.packet_start_code_prefix >> 8;

        if(0x00000001 != PES_packet_temp.packet_start_code_prefix)
        {
            return;
        }

        p_RTP_data->payload_start_position += 3;
        p_RTP_data->payload_size -= 3;

        PES_packet_temp.stream_id = *(p_RTP_data->payload_start_position);

        p_RTP_data->payload_start_position++;
        p_RTP_data->payload_size--;

        for(size_t i = 0; i < sizeof(uint16_t); i++)
        {
            ((uint8_t*)(&PES_packet_temp.PES_packet_length))[i] = p_RTP_data->payload_start_position[sizeof(uint16_t) - i - 1];
        }

        p_RTP_data->payload_start_position += 2;
        p_RTP_data->payload_size -= 2;

        if(0xE0 <= PES_packet_temp.stream_id && PES_packet_temp.stream_id <= 0xEF)
        {
            break;
        }

        p_RTP_data->payload_start_position += PES_packet_temp.PES_packet_length;
        p_RTP_data->payload_size -= PES_packet_temp.PES_packet_length;
    }

    if(PROGRAM_STREAM_MAP______ != PES_packet_temp.stream_id &&
       PADDING_STREAM__________ != PES_packet_temp.stream_id &&
       PRIVATE_STREAM_2________ != PES_packet_temp.stream_id &&
       ECM_____________________ != PES_packet_temp.stream_id &&
       EMM_____________________ != PES_packet_temp.stream_id &&
       PROGRAM_STREAM_DIRECTORY != PES_packet_temp.stream_id &&
       DSMCC_STREAM____________ != PES_packet_temp.stream_id &&
       ITU_T_REC_H_222_1_TYPE_E != PES_packet_temp.stream_id)
    {
        // to do
        return;
    }
    else if(PROGRAM_STREAM_MAP______ == PES_packet_temp.stream_id &&
            PRIVATE_STREAM_2________ == PES_packet_temp.stream_id &&
            ECM_____________________ == PES_packet_temp.stream_id &&
            EMM_____________________ == PES_packet_temp.stream_id &&
            PROGRAM_STREAM_DIRECTORY == PES_packet_temp.stream_id &&
            DSMCC_STREAM____________ == PES_packet_temp.stream_id &&
            ITU_T_REC_H_222_1_TYPE_E == PES_packet_temp.stream_id)
    {

    }
    else if(PADDING_STREAM__________ == PES_packet_temp.stream_id)
    {

    }


}