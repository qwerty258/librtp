#pragma once
#ifndef _LIBRTP_CALLBACK_DEFINE_H_
#define _LIBRTP_CALLBACK_DEFINE_H_

#include <stdint.h>
#include "libRTPSessionHandleDefine.h"

typedef int(*function_give_out_payload)(RTP_session_handle session_handle, uint8_t* payload, uint32_t payload_size, uint16_t sequence_number, uint32_t timestamp);

#endif // !_LIBRTP_CALLBACK_DEFINE_H_
