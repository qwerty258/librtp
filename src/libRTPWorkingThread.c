#include "libRTPSessions.h"
#include "libRTPDataDefines.h"
#include "libRTPMemory.h"
#include "libRTPWorkingThread.h"

uint32_t WINAPI RTP_receiving_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    p_RTP_session_context->local_sockaddr.sin_addr.S_un.S_addr = inet_addr(p_RTP_session_context->local_IPv4);
    p_RTP_session_context->local_sockaddr.sin_family = p_RTP_session_context->IP_version;
    p_RTP_session_context->local_sockaddr.sin_port = htons(p_RTP_session_context->local_port);

    p_RTP_session_context->remote_sockaddr.sin_addr.S_un.S_addr = inet_addr(p_RTP_session_context->remote_IPv4);
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

    free_concurrent_queue(p_RTP_session_context->raw_socket_data_queue_handle);

    result = close(sock);
    if(0 != result)
    {
        // to do
    }

    return 0;
}

