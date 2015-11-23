#include "libRTPPayloadProcessors.h"
#include "libRTPMemory.h"
#include "libRTPH264PayloadProcessingThread.h"
#include "libRTPPSPayloadProcessingThread.h"

payload_processor_context* get_payload_processor_table(void)
{
    payload_processor_context* temp = libRTP_calloc(10 * sizeof(payload_processor_context));
    temp[0].payload_type = MAKEFOURCC('H', '2', '6', '4');
    temp[0].p_function_payload_processing_thread = H264_payload_processing_thread;
    temp[1].payload_type = MAKEFOURCC(' ', ' ', 'P', 'S');
    temp[1].p_function_payload_processing_thread = PS_payload_processing_thread;
    temp[2].payload_type = 0;
    temp[2].p_function_payload_processing_thread = 0;
    return temp;
}
