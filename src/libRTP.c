#include "libRTP.h"
#include "libRTPSessions.h"
#include "libRTPMemory.h"
#include "libRTPWorkingThread.h"
#include "libRTPSocket.h"
#include "libRTPCheckMacros.h"

RTP_session_context** global_RTP_session_context_pointer_array;
size_t global_RTP_max_session_number;
payload_processor_context* global_payload_processor_table;

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
        return LIBRTP_SOCKET_ERROR;
    }
#else
#endif // _WIN32

    global_RTP_max_session_number = max_session_number;
    global_RTP_session_context_pointer_array = libRTP_calloc(global_RTP_max_session_number * sizeof(RTP_session_context*));
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array);

    global_payload_processor_table = get_payload_processor_table();
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_payload_processor_table);
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
    libRTP_free(global_payload_processor_table);

#ifdef _WIN32
    int result = WSACleanup();
    if(0 != result)
    {
        return LIBRTP_SOCKET_ERROR;
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
            global_RTP_session_context_pointer_array[i]->this_session_handle = i;
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
    int result = 0;
    CHECK_HANDLE(handle);
    if(global_RTP_session_context_pointer_array[handle]->session_started)
    {
        global_RTP_session_context_pointer_array[handle]->session_started = false;

#ifdef _WIN32
        WaitForSingleObject(global_RTP_session_context_pointer_array[handle]->RTP_receiving_thread_handle, INFINITE);
        CloseHandle(global_RTP_session_context_pointer_array[handle]->RTP_receiving_thread_handle);

        WaitForSingleObject(global_RTP_session_context_pointer_array[handle]->RTCP_thread_handle, INFINITE);
        CloseHandle(global_RTP_session_context_pointer_array[handle]->RTCP_thread_handle);

        WaitForSingleObject(global_RTP_session_context_pointer_array[handle]->Unpack_RTP_header_thread_handle, INFINITE);
        CloseHandle(global_RTP_session_context_pointer_array[handle]->Unpack_RTP_header_thread_handle);

        WaitForSingleObject(global_RTP_session_context_pointer_array[handle]->RTP_payload_processing_thread_handle, INFINITE);
        CloseHandle(global_RTP_session_context_pointer_array[handle]->RTP_payload_processing_thread_handle);
#endif // _WIN32

        result = close_sockets(global_RTP_session_context_pointer_array[handle]);
    }

    concurrent_queue_free(&global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_raw_socket_data);
    concurrent_queue_free(&global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_payload);

    libRTP_free(global_RTP_session_context_pointer_array[handle]->local_IPv4);
    libRTP_free(global_RTP_session_context_pointer_array[handle]->local_IPV6);
    libRTP_free(global_RTP_session_context_pointer_array[handle]->remote_IPv4);
    libRTP_free(global_RTP_session_context_pointer_array[handle]->remote_IPv6);

    libRTP_free(global_RTP_session_context_pointer_array[handle]);
    global_RTP_session_context_pointer_array[handle] = NULL;

    return result;
}

LIBRTP_API int set_RTP_session_IP_version(RTP_session_handle handle, uint32_t version)
{
    CHECK_HANDLE(handle);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->IP_version = version;
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_local_IPv4(RTP_session_handle handle, char* IPv4)
{
    CHECK_HANDLE(handle);
    CHECK_NULL_PARAMETER_AND_RETURN(IPv4);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->local_IPv4 = libRTP_strdup(IPv4);
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array[handle]->local_IPv4);
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_local_port(RTP_session_handle handle, uint16_t port)
{
    CHECK_HANDLE(handle);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->local_port = port;
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_remote_IPv4(RTP_session_handle handle, char* IPv4)
{
    CHECK_HANDLE(handle);
    CHECK_NULL_PARAMETER_AND_RETURN(IPv4);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->remote_IPv4 = libRTP_strdup(IPv4);
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array[handle]->remote_IPv4);
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_remote_port(RTP_session_handle handle, uint16_t port)
{
    CHECK_HANDLE(handle);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->remote_port = port;
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_IP_protocol(RTP_session_handle handle, uint32_t protocol)
{
    CHECK_HANDLE(handle);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->IP_protocol = protocol;
    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_payload_type(RTP_session_handle handle, uint32_t payload_type)
{
    CHECK_HANDLE(handle);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->payload_type = payload_type;
    size_t i = 0;
    while(true)
    {
        if(payload_type == global_payload_processor_table[i].payload_type ||
           0 == global_payload_processor_table[i].payload_type)
        {
            break;
        }
        i++;
    }

    if(NULL == global_payload_processor_table[i].p_function_payload_processing_thread)
    {
        return LIBRTP_PAYLOAD_UNSUPPORTED;
    }
    else
    {
        global_RTP_session_context_pointer_array[handle]->p_function_payload_processing_thread =
            global_payload_processor_table[i].p_function_payload_processing_thread;
    }

    return LIBRTP_OK;
}

LIBRTP_API int RTP_session_start(RTP_session_handle handle)
{
    int result;
    CHECK_HANDLE(handle);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);

    global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_raw_socket_data = concurrent_queue_get_handle();
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_raw_socket_data);
    global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_payload = concurrent_queue_get_handle();
    CHECK_MEMORY_ALLOCATE_RESULT_AND_RETURN(global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_payload);

    result = initialize_sockets(global_RTP_session_context_pointer_array[handle]);
    if(LIBRTP_OK != result)
    {
        concurrent_queue_free(&global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_raw_socket_data);
        concurrent_queue_free(&global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_payload);
        close_sockets(global_RTP_session_context_pointer_array[handle]);
        return result;
    }

    global_RTP_session_context_pointer_array[handle]->session_started = true;

#ifdef _WIN32
    global_RTP_session_context_pointer_array[handle]->RTP_receiving_thread_handle = CreateThread(
        NULL,
        0,
        RTP_receiving_thread,
        global_RTP_session_context_pointer_array[handle],
        0,
        &result);

    global_RTP_session_context_pointer_array[handle]->RTCP_thread_handle = CreateThread(
        NULL,
        0,
        RTCP_thread,
        global_RTP_session_context_pointer_array[handle],
        0,
        &result);

    global_RTP_session_context_pointer_array[handle]->Unpack_RTP_header_thread_handle = CreateThread(
        NULL,
        0,
        Unpack_RTP_header,
        global_RTP_session_context_pointer_array[handle],
        0,
        &result);

    global_RTP_session_context_pointer_array[handle]->RTP_payload_processing_thread_handle = CreateThread(
        NULL,
        0,
        global_RTP_session_context_pointer_array[handle]->p_function_payload_processing_thread,
        global_RTP_session_context_pointer_array[handle],
        0,
        &result);
    if(NULL == global_RTP_session_context_pointer_array[handle]->RTP_receiving_thread_handle ||
       NULL == global_RTP_session_context_pointer_array[handle]->RTCP_thread_handle ||
       NULL == global_RTP_session_context_pointer_array[handle]->Unpack_RTP_header_thread_handle ||
       NULL == global_RTP_session_context_pointer_array[handle]->RTP_payload_processing_thread_handle)
    {
        global_RTP_session_context_pointer_array[handle]->session_started = false;
        concurrent_queue_free(&global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_raw_socket_data);
        concurrent_queue_free(&global_RTP_session_context_pointer_array[handle]->concurrent_queue_handle_for_payload);
        close_sockets(global_RTP_session_context_pointer_array[handle]);
        return LIBRTP_THREAD_CREATE_ERROR;
    }
#else
#endif // _WIN32

    return LIBRTP_OK;
}

LIBRTP_API int set_RTP_session_payload_give_out_callback(RTP_session_handle handle, function_give_out_payload p_function, void* user_data)
{
    CHECK_HANDLE(handle);
    CHECK_NULL_PARAMETER_AND_RETURN(p_function);
    CHECK_SESSION_STARTED_NO_SET(global_RTP_session_context_pointer_array[handle]->session_started);
    global_RTP_session_context_pointer_array[handle]->p_function_give_out_payload = p_function;
    global_RTP_session_context_pointer_array[handle]->user_data = user_data;
    return LIBRTP_OK;
}