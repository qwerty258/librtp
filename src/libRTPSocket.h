#pragma once
#ifndef _LIBRTP_SOCKET_H_
#define _LIBRTP_SOCKET_H_

#include "libRTPSessions.h"

int initialize_sockets(RTP_session_context* p_session_context);

int close_sockets(RTP_session_context* p_session_context);

#endif // !_LIBRTP_SOCKET_H_
