#pragma once
#ifndef _LIBRTP_SESSIONS_H_
#define _LIBRTP_SESSIONS_H_

#include <stdint.h>

#ifdef _WIN32
#include <WinSock2.h>
#else
#endif // _WIN32

#ifdef _WIN32
typedef SOCKET RTP_socket;
typedef HANDLE RTP_thread_handl;
#else
#endif // _WIN32

typedef struct _RTP_session_context
{
    RTP_socket sock;
    uint32_t IP_version;
    char* local_IPv4;
    char* remote_IPv4;
    uint16_t local_port;
    uint16_t remote_port;
    uint32_t IP_protocol;
    struct sockaddr_in local_sockaddr;
    struct sockaddr_in remote_sockaddr;
    RTP_thread_handl thread_handle;
    uint32_t thread_ID;
}RTP_session_context;

#endif // !_LIBRTP_SESSIONS_H_
