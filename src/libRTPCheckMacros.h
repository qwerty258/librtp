#pragma once
#ifndef _LIBRTP_CHECK_MACROS_H_
#define _LIBRTP_CHECK_MACROS_H_

#include "libRTPErrorDefine.h"

#define CHECK_SESSION_STARTED_NO_SET(bool_status)   \
if(bool_status)                                     \
{                                                   \
    return LIBRTP_SESSION_ALREADY_STARTED;          \
}

#define CHECK_HANDLE(handle)                                    \
if(global_RTP_max_session_number <= (handle) ||                 \
   NULL == global_RTP_session_context_pointer_array[(handle)])  \
{                                                               \
    return LIBRTP_INVALID_SESSION_HANDLE;                       \
}

#define CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(pointer)    \
if(NULL == (pointer))                                       \
{                                                           \
    return LIBRTP_MEMORY_ERROR;                             \
}                                                           \

#define CHECK_NULL_PARAMETER_AND_RETURN(pointer)    \
if(NULL == (pointer))                               \
{                                                   \
    return LIBRTP_BAD_PARAMETER;                    \
}



#endif // !_LIBRTP_CHECK_MACROS_H_
