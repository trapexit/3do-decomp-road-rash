#include "data_stream_server_runtime.h"

DSRequestMsgPtr
take_next_data_stream_request(DSStreamCBPtr stream)
{
  DSRequestMsgPtr request;

  request = stream->requestMsgHead;
  if(request != 0)
    {
      stream->requestMsgHead = (DSRequestMsgPtr)request->link;
      if(stream->requestMsgTail == request)
        {
          stream->requestMsgTail = 0;
        }
    }
  return request;
}
