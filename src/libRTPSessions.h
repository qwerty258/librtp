#pragma once
#ifndef _LIBRTP_SESSIONS_H_
#define _LIBRTP_SESSIONS_H_

#include "libRTPConcurrentQueue.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
#include <WinSock2.h>
#else
#endif // _WIN32

#include "libRTPPayloadProcessors.h"
#include "libRTPCallbackDefine.h"

#ifdef _WIN32
typedef SOCKET RTP_socket;
typedef HANDLE RTP_thread_handle;
#define close closesocket
#else
#endif // _WIN32

typedef struct _RTP_session_context
{
    // session status begin
    bool session_started;
    size_t this_session_handle;
    // session status end

    // session socket begin
    uint32_t IP_version;
    uint32_t IP_protocol;
    char* local_IPv4;
    char* local_IPV6;
    char* remote_IPv4;
    char* remote_IPv6;
    uint16_t local_port;
    uint16_t remote_port;
    RTP_socket sock_for_RTP;
    RTP_socket sock_for_RTCP;
    struct sockaddr_in local_sockaddr_v4_for_RTP; // to do: IPv6 addr for RTP
    struct sockaddr_in remote_sockaddr_v4_for_RTP;
    struct sockaddr_in local_sockaddr_v4_for_RTCP; // to do: IPv6 addr for RTCP
    struct sockaddr_in remote_sockaddr_v4_for_RTCP;
    // session socket end

    // session receiving begin
    RTP_thread_handle RTP_receiving_thread_handle;
    RTP_thread_handle RTCP_thread_handle;
    RTP_thread_handle RTP_package_consuming_thread_handle;
    concurrent_queue_handle concurrent_queue_handle_for_raw_socket_data;
    // session receiving end

    payload_processer_function p_payload_processer_function;
    function_give_out_payload p_function_give_out_payload;
}RTP_session_context;

#endif // !_LIBRTP_SESSIONS_H_
