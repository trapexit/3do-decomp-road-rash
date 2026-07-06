#include "data_stream_server_runtime.h"

DSDataBufPtr
take_filled_data_stream_buffer(DSStreamCBPtr stream)
{
  DSDataBufPtr buffer;

  buffer = stream->filledBufHead;
  if(buffer != 0)
    {
      stream->filledBufHead = buffer->next;
      if(stream->filledBufTail == buffer)
        {
          stream->filledBufTail = 0;
        }
    }
  return buffer;
}
