#include "libRTPSocket.h"
#include "libRTPErrorDefine.h"

uint32_t libRTP_inet_addr(const char* IPv4)
{
    if(NULL == IPv4)
    {
        return inet_addr("0.0.0.0");
    }
    else
    {
        return inet_addr(IPv4);
    }
}

int initialize_sockets(RTP_session_context* p_session_context)
{
    int32_t result = 0;
    uint32_t timeout = 1000;

    if((AF_INET == p_session_context->IP_version && NULL == p_session_context->local_IPv4) ||
       (AF_INET6 == p_session_context->IP_version && NULL == p_session_context->local_IPV6))
    {
        return LIBRTP_SOCKET_ERROR;
    }

    if(AF_INET == p_session_context->IP_version)
    {
        p_session_context->local_sockaddr_v4_for_RTP.sin_addr.S_un.S_addr = libRTP_inet_addr(p_session_context->local_IPv4);
        p_session_context->local_sockaddr_v4_for_RTP.sin_family = AF_INET;
        p_session_context->local_sockaddr_v4_for_RTP.sin_port = htons(p_session_context->local_port);

        p_session_context->remote_sockaddr_v4_for_RTP.sin_addr.S_un.S_addr = libRTP_inet_addr(p_session_context->remote_IPv4);
        p_session_context->remote_sockaddr_v4_for_RTP.sin_family = AF_INET;
        p_session_context->remote_sockaddr_v4_for_RTP.sin_port = htons(p_session_context->local_port);

        p_session_context->local_sockaddr_v4_for_RTCP.sin_addr.S_un.S_addr = libRTP_inet_addr(p_session_context->local_IPv4);
        p_session_context->local_sockaddr_v4_for_RTCP.sin_family = AF_INET;
        p_session_context->local_sockaddr_v4_for_RTCP.sin_port = htons(p_session_context->local_port + 1);

        p_session_context->remote_sockaddr_v4_for_RTCP.sin_addr.S_un.S_addr = libRTP_inet_addr(p_session_context->remote_IPv4);
        p_session_context->remote_sockaddr_v4_for_RTCP.sin_family = AF_INET;
        p_session_context->remote_sockaddr_v4_for_RTCP.sin_port = htons(p_session_context->remote_port + 1);

        if(IPPROTO_UDP == p_session_context->IP_protocol)
        {
            p_session_context->sock_for_RTP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            p_session_context->sock_for_RTCP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        }
        else if(IPPROTO_TCP == p_session_context->IP_protocol)
        {
            p_session_context->sock_for_RTP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            p_session_context->sock_for_RTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        }
        else
        {
            return LIBRTP_SOCKET_ERROR;
        }
        if((RTP_socket)(~0) == p_session_context->sock_for_RTP || (RTP_socket)(~0) == p_session_context->sock_for_RTCP)
        {
            return LIBRTP_SOCKET_ERROR;
        }

        result = bind(
            p_session_context->sock_for_RTP,
            (struct sockaddr*)&p_session_context->local_sockaddr_v4_for_RTP,
            sizeof(struct sockaddr_in));
        if(0 != result)
        {
            return LIBRTP_SOCKET_ERROR;
        }

        result = bind(
            p_session_context->sock_for_RTCP,
            (struct sockaddr*)&p_session_context->local_sockaddr_v4_for_RTCP,
            sizeof(struct sockaddr_in));
        if(0 != result)
        {
            return LIBRTP_SOCKET_ERROR;
        }

        result = setsockopt(
            p_session_context->sock_for_RTP,
            SOL_SOCKET,
            SO_RCVTIMEO,
            (const char*)&timeout,
            sizeof(uint32_t));
        if(0 != result)
        {
            return LIBRTP_SOCKET_ERROR;
        }

        result = setsockopt(
            p_session_context->sock_for_RTCP,
            SOL_SOCKET,
            SO_RCVTIMEO,
            (const char*)&timeout,
            sizeof(uint32_t));
        if(0 != result)
        {
            return LIBRTP_SOCKET_ERROR;
        }

        return LIBRTP_OK;
    }
    else if(AF_INET6 == p_session_context->IP_version)
    {
        // to do
        return LIBRTP_SOCKET_ERROR;
    }
    else
    {
        return LIBRTP_SOCKET_ERROR;
    }
}

int close_sockets(RTP_session_context* p_session_context)
{
    uint32_t close_sock_for_RTCP;
    uint32_t close_sock_for_RTP;
    close_sock_for_RTCP = close(p_session_context->sock_for_RTCP);
    close_sock_for_RTP = close(p_session_context->sock_for_RTP);
    if(0 != close_sock_for_RTCP || 0 != close_sock_for_RTP)
    {
        return LIBRTP_SOCKET_ERROR;
    }
    else
    {
        return LIBRTP_OK;
    }
}