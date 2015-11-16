#include "libRTPWorkingThread.h"
#include "libRTPSessions.h"

uint32_t receiving_thread(void* parameter)
{
    RTP_session_context* p_RTP_session_context = (RTP_session_context*)parameter;
    p_RTP_session_context->local_sockaddr.sin_addr.S_un.S_addr = inet_addr(p_RTP_session_context->local_IPv4);
    p_RTP_session_context->local_sockaddr.sin_family = p_RTP_session_context->IP_version;
    p_RTP_session_context->local_sockaddr.sin_port = htons(p_RTP_session_context->local_port);

    int result = bind(
        p_RTP_session_context->sock,
        (struct sockaddr*)&p_RTP_session_context->local_sockaddr,
        sizeof(struct sockaddr_in));
    if(0 == result)
    {
        // to do
    }

    return 0;
}

