#include "libRTP.h"
#include "libRTPSessions.h"
#include "libRTPMemory.h"
#include "libRTPWorkingThread.h"

#define CHECK_SESSION_STARTED_NO_SET(bool_status)   \
if(bool_status)                                     \
{                                                   \
    return LIBRTP_SESSION_ALREADY_STARTED;          \
}

RTP_session_context** global_RTP_session_context_pointer_array;
size_t global_RTP_max_session_number;

int check_handle(size_t handle)
{
    if(global_RTP_max_session_number <= handle || NULL == global_RTP_session_context_pointer_array[handle])
    {
        return LIBRTP_INVALID_SESSION_HANDLE;
    }
    else
    {
        return LIBRTP_OK;
    }
}

LIBRTP_API int initial_RTP_library(size_t max_session_number)
{
    if(0 == max_session_number)
    {
        return LIBRTP_UNDEFINED_ERROR;
    }
#ifdef _WIN32
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if(0 != result)
    {
        // to do: handle error
    }
#else
#endif // _WIN32

    global_RTP_max_session_number = max_session_number;
    global_RTP_session_context_pointer_array = libRTP_calloc(global_RTP_max_session_number * sizeof(RTP_session_context*));

    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array);
    return LIBRTP_OK;
}

LIBRTP_API int uninitial_RTP_library(void)
{
    for(size_t i = 0; i < global_RTP_max_session_number; i++)
    {
        if(NULL != global_RTP_session_context_pointer_array[i])
        {
            close_RTP_session(i);
        }
    }

    libRTP_free(global_RTP_session_context_pointer_array);

#ifdef _WIN32
    int result = WSACleanup();
    if(0 != result)
    {
        // to do: handle error
    }
#else
#endif // _WIN32

    return LIBRTP_OK;
}

LIBRTP_API int get_new_RTP_session(RTP_session_handle* p_handle)
{
    CHECK_NULL_PARAMETER_AND_RETURN(p_handle);
    size_t i;
    for(i = 0; i < global_RTP_max_session_number; i++)
    {
        if(NULL == global_RTP_session_context_pointer_array[i])
        {
            global_RTP_session_context_pointer_array[i] = libRTP_calloc(sizeof(RTP_session_context));
            CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array[i]);
            *p_handle = i;
            break;
        }
    }
    if(i < global_RTP_max_session_number)
    {
        return LIBRTP_OK;
    }
    else
    {
        return LIBRTP_SESSION_HANDLE_DEPLETED;
    }
}

LIBRTP_API int close_RTP_session(RTP_session_handle handle)
{
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    global_RTP_session_context_pointer_array[handle]->session_started = false;

#ifdef _WIN32
    WaitForSingleObject(global_RTP_session_context_pointer_array[handle]->receiving_thread_handle, INFINITE);
    CloseHandle(global_RTP_session_context_pointer_array[handle]->receiving_thread_handle);
#endif // _WIN32

    libRTP_free(global_RTP_session_context_pointer_array[handle]);
    global_RTP_session_context_pointer_array[handle] = NULL;

    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_IP_version(RTP_session_handle handle, uint32_t version)
{
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->IP_version = version;
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_local_IPv4(RTP_session_handle handle, char* IPv4)
{
    CHECK_NULL_PARAMETER_AND_RETURN(IPv4);
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->local_IPv4 = libRTP_strdup(IPv4);
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array[handle]->local_IPv4);
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_local_port(RTP_session_handle handle, uint16_t port)
{
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->local_port = port;
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_remote_IPv4(RTP_session_handle handle, char* IPv4)
{
    CHECK_NULL_PARAMETER_AND_RETURN(IPv4);
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->remote_IPv4 = libRTP_strdup(IPv4);
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array[handle]->remote_IPv4);
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_remote_port(RTP_session_handle handle, uint16_t port)
{
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->remote_port = port;
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_IP_protocol(RTP_session_handle handle, uint32_t protocol)
{
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->IP_protocol = protocol;
    return LIBRTP_OK;
}

LIBRTP_API int RTP_session_start(RTP_session_handle handle)
{
    int result = check_handle(handle);
    if(LIBRTP_OK != result)
    {
        return result;
    }
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    if(IPPROTO_UDP == global_RTP_session_context_pointer_array[handle]->IP_protocol)
    {
        global_RTP_session_context_pointer_array[handle]->sock = socket(
            global_RTP_session_context_pointer_array[handle]->IP_version,
            SOCK_DGRAM,
            global_RTP_session_context_pointer_array[handle]->IP_protocol);
    }
    if(IPPROTO_TCP == global_RTP_session_context_pointer_array[handle]->IP_protocol)
    {
        global_RTP_session_context_pointer_array[handle]->sock = socket(
            global_RTP_session_context_pointer_array[handle]->IP_version,
            SOCK_STREAM,
            global_RTP_session_context_pointer_array[handle]->IP_protocol);
    }
    if((RTP_socket)(~0) == global_RTP_session_context_pointer_array[handle]->sock)
    {
        // to do: handle error
    }

#ifdef _WIN32
    global_RTP_session_context_pointer_array[handle]->receiving_thread_handle = CreateThread(
        NULL,
        0,
        RTP_receiving_thread,
        global_RTP_session_context_pointer_array[handle],
        0,
        &global_RTP_session_context_pointer_array[handle]->receiving_thread_ID);
#else
#endif // _WIN32

    return LIBRTP_OK;
}