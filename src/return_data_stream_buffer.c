#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"

int32
return_data_stream_buffer(DSStreamCBPtr stream,
                          DSDataBufPtr  buffer)
{
  int32 free_buffer_count;
  DSRequestMsgPtr end_request;

  buffer->next = stream->freeBufHead;
  stream->freeBufHead = buffer;

  free_buffer_count = stream->currentFreeBufferCount + 1;
  stream->currentFreeBufferCount = free_buffer_count;

  end_request = stream->endOfStreamMsg;
  if(end_request == 0)
    {
      return free_buffer_count;
    }
  if((stream->streamFlags & STRM_EOF) == 0)
    {
      return 0;
    }
  if(free_buffer_count != stream->totalBufferCount)
    {
      return free_buffer_count;
    }

  ReplyMsg(end_request->msgItem, 0, end_request,
           sizeof(DSRequestMsg));
  stream->endOfStreamMsg = 0;
  return 0;
}
