#include "data_stream_server_runtime.h"

void
queue_data_stream_request(DSStreamCBPtr   stream,
                          DSRequestMsgPtr request)
{
  request->link = 0;
  if(stream->requestMsgHead == 0)
    {
      stream->requestMsgHead = request;
    }
  else
    {
      stream->requestMsgTail->link = request;
    }
  stream->requestMsgTail = request;
}
