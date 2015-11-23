#pragma once
#ifndef _LIBRTP_EXPORT_H_
#define _LIBRTP_EXPORT_H_

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

#endif // !_LIBRTP_EXPORT_H_
