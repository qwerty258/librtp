#include "libRTPH264PayloadProcessor.h"
#include "libRTPPayloadProcessors.h"
#include "libRTPPSPayloadProcessor.h"
#include "libRTPMemory.h"

payload_processor_context* get_payload_processor_table(void)
{
    payload_processor_context* temp = libRTP_calloc(10 * sizeof(payload_processor_context));
    temp[0].payload_type = MAKEFOURCC('H', '2', '6', '4');
    temp[0].p_function = H264_payload_processor;
    temp[1].payload_type = MAKEFOURCC(' ', ' ', 'P', 'S');
    temp[1].p_function = PS_payload_processor;
    temp[2].payload_type = 0;
    temp[2].p_function = 0;
    return temp;
}
