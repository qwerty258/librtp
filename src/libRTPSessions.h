#ifndef _LIBRTP_SESSIONS_H_
#define _LIBRTP_SESSIONS_H_

#ifdef _WIN32
#include <WinSock2.h>
#else
#endif // _WIN32

#include <stdint.h>

#ifdef _WIN32
typedef SOCKET RTP_socket;
#else
#endif // _WIN32

typedef struct _RTP_session
{
    RTP_socket sock;
    uint32_t local_IPv4;
    uint32_t remote_IPv4;
    uint16_t local_port;
    uint16_t remote_port;
    uint32_t IP_protocol;
}RTP_session;

#endif // !_LIBRTP_SESSIONS_H_
