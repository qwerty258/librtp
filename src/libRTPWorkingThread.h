#pragma once
#ifndef _LIBRTP_WORKING_THREAD_H_
#define _LIBRTP_WORKING_THREAD_H_

#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#else
#define WINAPI
#endif // _WIN32

uint32_t WINAPI RTP_receiving_thread(void* parameter);
uint32_t WINAPI RTCP_thread(void* parameter);
uint32_t WINAPI Unpack_RTP_header(void* parameter);

#endif // !_LIBRTP_WORKING_THREAD_H_
