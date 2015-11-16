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

typedef void* RTP_session;

#define LIBRTP_OK               0;
#define LIBRTP_UNDEFINED_ERROR  -1;

C_EXPORT_BEGIN

LIBRTP_API int initial_RTP_library(void);

LIBRTP_API int uninitial_RTP_library(void);

LIBRTP_API RTP_session* get_new_RTP_session(void);

LIBRTP_API void close_RTP_session(RTP_session* session);

C_EXPORT_END

#endif // !_LIBRTP_H_