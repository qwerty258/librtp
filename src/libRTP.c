#include "libRTP.h"
#include "libRTPSessions.h"
#include "libRTPMemory.h"

LIBRTP_API int initial_RTP_library(void)
{
#ifdef _WIN32
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if(0 != result)
    {
        // to do: handle error
    }
#else
#endif // _WIN32
    return LIBRTP_OK;
}

LIBRTP_API int uninitial_RTP_library(void)
{
#ifdef _WIN32
    int result = WSACleanup();
    if(0 != result)
    {
        // to do: handle error
    }
#else
#endif // _WIN32
    return 0;
}

LIBRTP_API RTP_session* get_new_RTP_session(void)
{
    return libRTP_calloc(sizeof(RTP_session));
}

LIBRTP_API void close_RTP_session(RTP_session* session)
{
    libRTP_free(session);
}