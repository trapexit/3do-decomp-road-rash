#include "data_stream_server_runtime.h"

DSDataBufPtr
take_free_data_stream_buffer(DSStreamCBPtr stream)
{
  DSDataBufPtr buffer;

  buffer = stream->freeBufHead;
  if(buffer != 0)
    {
      stream->freeBufHead = buffer->next;
      buffer->streamCBPtr = stream;
      buffer->useCount = 0;
      stream->currentFreeBufferCount--;
    }
  return buffer;
}
