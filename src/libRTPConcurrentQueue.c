#include "libRTPConcurrentQueue.h"
#include "libRTPMemory.h"
#include <Windows.h>

typedef struct _concurrent_queue_node concurrent_queue_node;
typedef struct _concurrent_queue_node
{
    void* payload;
    concurrent_queue_node* p_next;
}concurrent_queue_node;

typedef struct _concurrent_queue_context
{
    concurrent_queue_node* head;
    concurrent_queue_node* tail;
    CRITICAL_SECTION critical_section;
}concurrent_queue_context;

concurrent_queue_handle concurrent_queue_get_handle(void)
{
    concurrent_queue_context* queue_context = libRTP_calloc(sizeof(concurrent_queue_context));
    if(NULL != queue_context)
    {
        queue_context->head = NULL;
        queue_context->tail = NULL;
        InitializeCriticalSectionAndSpinCount(&queue_context->critical_section, 4000);
    }
    return queue_context;
}

bool concurrent_queue_pushback(concurrent_queue_handle handle, void* element)
{
    if(NULL == handle)
    {
        return false;
    }
    concurrent_queue_context* queue_context = (concurrent_queue_context*)handle;
    concurrent_queue_node* node = libRTP_calloc(sizeof(concurrent_queue_node));
    if(NULL == node)
    {
        return false;
    }
    else
    {
        node->payload = element;
        node->p_next = NULL;
    }
    bool bad_context = false;

    EnterCriticalSection(&queue_context->critical_section);
    if(NULL == queue_context->head && NULL == queue_context->tail)
    {
        queue_context->head = node;
        queue_context->tail = node;
    }
    else if(NULL != queue_context->head && NULL != queue_context->tail)
    {
        queue_context->tail->p_next = node;
        queue_context->tail = node;
    }
    else
    {
        bad_context = true;
    }
    LeaveCriticalSection(&queue_context->critical_section);

    if(bad_context)
    {
        libRTP_free(node);
        return false;
    }
    else
    {
        return true;
    }
}

void* concurrent_queue_pophead(concurrent_queue_handle handle)
{
    if(NULL == handle)
    {
        return NULL;
    }
    concurrent_queue_context* queue_context = (concurrent_queue_context*)handle;
    concurrent_queue_node* node_temp = NULL;
    bool bad_context = false;
    void* payload = NULL;

    EnterCriticalSection(&queue_context->critical_section);
    if(NULL != queue_context->head && NULL != queue_context->tail && queue_context->head != queue_context->tail)
    {
        payload = queue_context->head->payload;
        node_temp = queue_context->head;
        queue_context->head = node_temp->p_next;
        libRTP_free(node_temp);
    }
    else if(NULL != queue_context->head && NULL != queue_context->tail  && queue_context->head == queue_context->tail)
    {
        payload = queue_context->head->payload;
        libRTP_free(queue_context->head);
        queue_context->head = NULL;
        queue_context->tail = NULL;
    }
    else
    {
        bad_context = true;
    }
    LeaveCriticalSection(&queue_context->critical_section);

    if(bad_context)
    {
        return NULL;
    }
    else
    {
        return payload;
    }
}

void concurrent_queue_free(concurrent_queue_handle* handle)
{
    if(NULL == handle || NULL == (*handle))
    {
        return;
    }
    concurrent_queue_context* queue_context = (concurrent_queue_context*)(*handle);
    while(NULL != concurrent_queue_pophead(queue_context))
    {
        continue;
    }
    DeleteCriticalSection(&queue_context->critical_section);
    libRTP_free(queue_context);
    *handle = NULL;
}