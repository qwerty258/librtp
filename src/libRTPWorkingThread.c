#include "libRTPSessions.h"
#include "libRTPDataDefines.h"
#include "libRTPMemory.h"
#include "libRTPWorkingThread.h"
#include "libRTPUnpackRTPHeader.h"
#include <stdio.h>

uint32_t WINAPI RTP_receiving_thread(void* parameter)
{
    int32_t result;
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    if(NULL == p_RTP_session_context->concurrent_queue_handle_for_raw_socket_data)
    {
        return -1;
    }

    RTP_data* p_raw_socket_data = NULL;

    while(p_RTP_session_context->session_started)
    {
        p_raw_socket_data = libRTP_calloc(sizeof(RTP_data));
        if(NULL != p_raw_socket_data)
        {
            result = recv(p_RTP_session_context->sock_for_RTP, p_raw_socket_data->data, RTP_DATA_BUFFER_SIZE, 0);
            if(0 < result)
            {
                p_raw_socket_data->socket_received_size = result;
                if(!concurrent_queue_pushback(p_RTP_session_context->concurrent_queue_handle_for_raw_socket_data, p_raw_socket_data))
                {
                    libRTP_free(p_raw_socket_data);
                }
            }
            else
            {
                libRTP_free(p_raw_socket_data);
            }
        }
    }

    // pop out all the data and free
    while(true)
    {
        p_raw_socket_data = concurrent_queue_pophead(p_RTP_session_context->concurrent_queue_handle_for_raw_socket_data);
        if(NULL == p_raw_socket_data)
        {
            break;
        }
        else
        {
            libRTP_free(p_raw_socket_data);
        }
    }

    return 0;
}

uint32_t WINAPI RTP_package_consuming_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    RTP_data* raw_data = NULL;
    while(p_RTP_session_context->session_started)
    {
        raw_data = concurrent_queue_pophead(p_RTP_session_context->concurrent_queue_handle_for_raw_socket_data);
        if(NULL == raw_data)
        {
            Sleep(10);
        }
        else
        {
            unpack_RTP_header(raw_data);
            p_RTP_session_context->p_payload_processer_function(raw_data);
            for(size_t i = 0; i < 8; i++)
            {
                printf("%02X", raw_data->payload_start_position[i]);
            }
            printf("  ");
            printf("size: %u  ", raw_data->payload_size);
            printf("timestamp: %u  ", raw_data->timestamp);
            printf("seq: %u\n", raw_data->sequence_number);

            libRTP_free(raw_data);
            raw_data = NULL;
        }
    }
    return 0;
}

uint32_t WINAPI RTCP_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    int result = 0;
    uint8_t* buffer = libRTP_calloc(RTP_DATA_BUFFER_SIZE);

    size_t write_count = 0;
    char filename[MAX_PATH];

    while(p_RTP_session_context->session_started)
    {
        printf("RTCP_thread\n");
        result = recv(p_RTP_session_context->sock_for_RTCP, buffer, RTP_DATA_BUFFER_SIZE, 0);
        if(0 < result)
        {
            for(write_count = 0; write_count < 5; write_count++)
            {
                _snprintf(filename, MAX_PATH, "D:\\RTCPdata%u", write_count);
                FILE* pFile = fopen(filename, "wb");
                fwrite(buffer, result, 1, pFile);
                fclose(pFile);
            }

            for(size_t i = 0; i < 10; i++)
            {
                printf("%02X", buffer[i]);
            }
            printf("\n");
        }
    }

    libRTP_free(buffer);
    return 0;
}