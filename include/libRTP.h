#pragma once
#ifndef _LIBRTP_H_
#define _LIBRTP_H_

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

C_EXPORT_BEGIN

#include <stdint.h>
#include "libRTPErrorDefine.h"

typedef size_t RTP_session_handle;

LIBRTP_API int initial_RTP_library(size_t max_session_number);

LIBRTP_API int uninitial_RTP_library(void);

LIBRTP_API int get_new_RTP_session(RTP_session_handle* p_handle);

LIBRTP_API int close_RTP_session(RTP_session_handle handle);

#define LIBRTP_AF_INET  2
#define LIBRTP_AF_INET6 23

LIBRTP_API int set_RTP_session_IP_version(RTP_session_handle handle, uint32_t version);

LIBRTP_API int set_RTP_session_local_IPv4(RTP_session_handle handle, char* IPv4);

LIBRTP_API int set_RTP_session_local_port(RTP_session_handle handle, uint16_t port);

LIBRTP_API int set_RTP_session_remote_IPv4(RTP_session_handle handle, char* IPv4);

LIBRTP_API int set_RTP_session_remote_port(RTP_session_handle handle, uint16_t port);

#define LIBRTP_IP_PROTOCOL_TCP  6
#define LIBRTP_IP_PROTOCOL_UDP  17

LIBRTP_API int set_RTP_session_IP_protocol(RTP_session_handle handle, uint32_t protocol);

LIBRTP_API int RTP_session_start(RTP_session_handle handle);

C_EXPORT_END

#endif // !_LIBRTP_H_