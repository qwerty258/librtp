#pragma once
#ifndef _LIBRTP_PS_PAYLOAD_PROCESSING_THREAD_H_
#define _LIBRTP_PS_PAYLOAD_PROCESSING_THREAD_H_

#include <stdint.h>

#ifdef _WIN32
#include <Windows.h>
#else
#define WINAPI
#endif // _WIN32

uint32_t WINAPI PS_payload_processing_thread(void* parameter);

#endif // !_LIBRTP_PS_PAYLOAD_PROCESSING_THREAD_H_
