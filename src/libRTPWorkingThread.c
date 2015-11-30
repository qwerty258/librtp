#include "libRTPSessions.h"
#include "libRTPDataDefines.h"
#include "libRTPMemory.h"
#include "libRTPWorkingThread.h"
#include "libRTPUnpackRTPHeader.h"
#include "libRTPCheckMacros.h"
#include <stdio.h>

uint32_t WINAPI RTP_receiving_thread(void* parameter)
{
    int32_t result = 0;
    RTP_data* p_raw_socket_data = NULL;
    RTP_session_context* p_RTP_session_context = NULL;

    CHECK_NULL_PARAMETER_AND_RETURN(parameter);

    p_RTP_session_context = (RTP_session_context*)parameter;
    if(NULL == p_RTP_session_context->concurrent_queue_handle_for_raw_socket_data)
    {
        return -1;
    }

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

uint32_t WINAPI Unpack_RTP_header(void* parameter)
{
    RTP_data* raw_data = NULL;
    RTP_session_context* p_RTP_session_context = NULL;

    CHECK_NULL_PARAMETER_AND_RETURN(parameter);

    p_RTP_session_context = (RTP_session_context*)parameter;

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
            if(2 == raw_data->RTP_package_byte_0.little_endian.V)
            {
                if(!concurrent_queue_pushback(p_RTP_session_context->concurrent_queue_handle_for_payload, raw_data))
                {
                    libRTP_free(raw_data);
                }
            }
            else
            {
                libRTP_free(raw_data);
            }
        }
    }

    // pop out all the data and free
    while(true)
    {
        raw_data = concurrent_queue_pophead(p_RTP_session_context->concurrent_queue_handle_for_payload);
        if(NULL == raw_data)
        {
            break;
        }
        else
        {
            libRTP_free(raw_data);
        }
    }

    return 0;
}

uint32_t WINAPI RTCP_thread(void* parameter)
{
    size_t write_count = 0;
    int result = 0;
    RTP_session_context* p_RTP_session_context;
    uint8_t* buffer;
    char filename[MAX_PATH];

    CHECK_NULL_PARAMETER_AND_RETURN(parameter);
    p_RTP_session_context = (RTP_session_context*)parameter;
    buffer = libRTP_calloc(RTP_DATA_BUFFER_SIZE);
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(buffer);

    while(p_RTP_session_context->session_started)
    {
        result = recv(p_RTP_session_context->sock_for_RTCP, buffer, RTP_DATA_BUFFER_SIZE, 0);
        if(0 < result)
        {
            if(write_count < 5)
            {
                _snprintf(filename, MAX_PATH, "D:\\RTCPdata%u", write_count);
                FILE* pFile = fopen(filename, "wb");
                fwrite(buffer, result, 1, pFile);
                fclose(pFile);
                write_count++;
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