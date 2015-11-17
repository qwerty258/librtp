#include "libRTPSessions.h"
#include "libRTPDataDefines.h"
#include "libRTPMemory.h"
#include "libRTPWorkingThread.h"
#include <stdio.h>

uint32_t WINAPI RTP_receiving_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    if(NULL == p_RTP_session_context->local_IPv4)
    {
        p_RTP_session_context->local_sockaddr.sin_addr.S_un.S_addr = 0;
    }
    else
    {
        p_RTP_session_context->local_sockaddr.sin_addr.S_un.S_addr = inet_addr(p_RTP_session_context->local_IPv4);

    }
    p_RTP_session_context->local_sockaddr.sin_family = p_RTP_session_context->IP_version;
    p_RTP_session_context->local_sockaddr.sin_port = htons(p_RTP_session_context->local_port);

    if(NULL==p_RTP_session_context->remote_IPv4)
    {
        p_RTP_session_context->remote_sockaddr.sin_addr.S_un.S_addr = 0;
    }
    else
    {
        p_RTP_session_context->remote_sockaddr.sin_addr.S_un.S_addr = inet_addr(p_RTP_session_context->remote_IPv4);
    }
    p_RTP_session_context->remote_sockaddr.sin_family = p_RTP_session_context->IP_version;
    p_RTP_session_context->remote_sockaddr.sin_port = htons(p_RTP_session_context->local_port);

    int32_t result = bind(
        p_RTP_session_context->sock,
        (struct sockaddr*)&p_RTP_session_context->local_sockaddr,
        sizeof(struct sockaddr_in));
    if(0 != result)
    {
        // to do
    }

    int addrlen = sizeof(struct sockaddr_in);
    RTP_socket sock = p_RTP_session_context->sock;
    if(IPPROTO_TCP == p_RTP_session_context->IP_protocol)
    {
        sock = accept(
            p_RTP_session_context->sock,
            (struct sockaddr*)&p_RTP_session_context->remote_sockaddr,
            &addrlen);
        if((RTP_socket)(~0) == sock)
        {
            // to do
        }
    }

    uint32_t timeout = 1000;
    result = setsockopt(
        p_RTP_session_context->sock,
        SOL_SOCKET,
        SO_RCVTIMEO,
        (const char*)&timeout,
        sizeof(uint32_t));
    if(0 != result)
    {
        // to do
    }

    raw_socket_data* p_raw_socket_data = NULL;
    p_RTP_session_context->raw_socket_data_queue_handle = get_concurrent_queue();
    if(NULL == p_RTP_session_context->raw_socket_data_queue_handle)
    {
        // to do
    }

    HANDLE RTCP_thread_handle = CreateThread(NULL, 0, RTCP_thread, parameter, 0, &result);
    if(NULL == RTCP_thread_handle)
    {
        // to do
    }

    HANDLE RTP_package_consuming_thread_handle = CreateThread(NULL, 0, RTP_package_consuming_thread, parameter, 0, &result);
    if(NULL == RTP_package_consuming_thread_handle)
    {
        // to do
    }

    while(p_RTP_session_context->session_started)
    {
        p_raw_socket_data = libRTP_calloc(sizeof(raw_socket_data));
        if(NULL != p_raw_socket_data)
        {
            result = recv(sock, p_raw_socket_data->buffer, 2000, 0);
            if(0 < result)
            {
                p_raw_socket_data->size = result;
                if(!concurrent_pushback(p_RTP_session_context->raw_socket_data_queue_handle, p_raw_socket_data))
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

    WaitForSingleObject(RTCP_thread_handle, INFINITE);
    CloseHandle(RTCP_thread_handle);

    WaitForSingleObject(RTP_package_consuming_thread_handle, INFINITE);
    CloseHandle(RTP_package_consuming_thread_handle);

    free_concurrent_queue(p_RTP_session_context->raw_socket_data_queue_handle);

    result = close(sock);
    if(0 != result)
    {
        // to do
    }

    return 0;
}

uint32_t WINAPI RTP_package_consuming_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    char* raw_data = NULL;
    while(p_RTP_session_context->session_started)
    {
        raw_data = concurrent_pophead(p_RTP_session_context->raw_socket_data_queue_handle);
        if(NULL == raw_data)
        {
            Sleep(10);
        }
        else
        {
            for(size_t i = 0; i < 10; i++)
            {
                printf("%02X", raw_data[i]);
            }
            printf("\n");

            libRTP_free(raw_data);
        }
    }
    return 0;
}

uint32_t WINAPI RTCP_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    struct sockaddr_in local_RTCP_addr;
    int result = 0;
    uint8_t* buffer = libRTP_calloc(2000);
    memset(&local_RTCP_addr, 0x0, sizeof(struct sockaddr_in));

    local_RTCP_addr.sin_addr.S_un.S_addr = inet_addr(p_RTP_session_context->local_IPv4);
    local_RTCP_addr.sin_family = p_RTP_session_context->IP_version;
    local_RTCP_addr.sin_port = htons(p_RTP_session_context->local_port + 1);

    RTP_socket RTCP_socket;
    if(IPPROTO_UDP == p_RTP_session_context->IP_protocol)
    {
        RTCP_socket = socket(
            p_RTP_session_context->IP_version,
            SOCK_DGRAM,
            p_RTP_session_context->IP_protocol);
    }
    if(IPPROTO_TCP == p_RTP_session_context->IP_protocol)
    {
        RTCP_socket = socket(
            p_RTP_session_context->IP_version,
            SOCK_STREAM,
            p_RTP_session_context->IP_protocol);
    }
    if((RTP_socket)(~0) == RTCP_socket)
    {
        // to do
    }

    result = bind(RTCP_socket, (struct sockaddr*)&local_RTCP_addr, sizeof(struct sockaddr_in));
    if(0 != result)
    {
        // to do
    }

    uint32_t timeout = 1000;
    result = setsockopt(
        p_RTP_session_context->sock,
        SOL_SOCKET,
        SO_RCVTIMEO,
        (const char*)&timeout,
        sizeof(uint32_t));
    if(0 != result)
    {
        // to do
    }

    size_t write_count = 0;
    char filename[MAX_PATH];

    while(p_RTP_session_context->session_started)
    {
        result = recv(RTCP_socket, buffer, 2000, 0);
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