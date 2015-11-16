#pragma once
#ifndef _LIBRTP_H_
#define _LIBRTP_H_

#include <stdint.h>

#ifdef _MSC_VER
#ifdef LIBRTP_EXPORTS
#define LIBRTP_API __declspec(dllexport)
#else
#define LIBRTP_API __declspec(dllimport)
#endif // LIBRTP_EXPORTS
#else
#define LIBRTP_API
#endif // _WIN32

#ifdef __cplusplus
#define C_EXPORT_BEGIN extern "C" {
#define C_EXPORT_END }
#else
#define C_EXPORT_BEGIN
#define C_EXPORT_END
#endif // __cplusplus

typedef void* RTP_session;

#define LIBRTP_OK                       0
#define LIBRTP_UNDEFINED_ERROR          -1
#define LIBRTP_MEMORY_ERROR             -2
#define LIBRTP_BAD_PARAMETER            -3
#define LIBRTP_SESSION_CONFIG_ERROR     -4
#define LIBRTP_SESSION_ALREADY_STARTED  -5

C_EXPORT_BEGIN

LIBRTP_API int initial_RTP_library(void);

LIBRTP_API int uninitial_RTP_library(void);

LIBRTP_API RTP_session* get_new_RTP_session(void);

LIBRTP_API void close_RTP_session(RTP_session* session);

#define LIBRTP_AF_INET  2
#define LIBRTP_AF_INET6 23

LIBRTP_API int set_RTP_session_IP_version(RTP_session* session, uint32_t version);

LIBRTP_API int set_RTP_session_local_IPv4(RTP_session* session, char* IPv4);

LIBRTP_API int set_RTP_session_local_port(RTP_session* session, uint16_t port);

LIBRTP_API int set_RTP_session_remote_IPv4(RTP_session* session, char* IPv4);

LIBRTP_API int set_RTP_session_remote_port(RTP_session* session, uint16_t port);

#define LIBRTP_IP_PROTOCOL_TCP  6
#define LIBRTP_IP_PROTOCOL_UDP  17

LIBRTP_API int set_RTP_session_IP_protocol(RTP_session* session, uint32_t protocol);

LIBRTP_API int RTP_session_start(RTP_session* session);

C_EXPORT_END

#endif // !_LIBRTP_H_