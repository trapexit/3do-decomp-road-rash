#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"

int32
release_subscriber_data_stream_buffer(DSStreamCBPtr stream,
                                      int32         subscriber_status,
                                      DSDataBufPtr  buffer)
{
  DSStreamCBPtr owning_stream;
  int32 result;

  owning_stream = stream;
  result = ROADRASH_DS_NO_ERROR;

  buffer->useCount--;
  if(buffer->useCount == 0)
    {
      owning_stream = buffer->streamCBPtr;
      if(subscriber_status == ROADRASH_DS_NO_ERROR)
        {
          result = request_data_stream_buffer_fill(
            owning_stream, buffer);
        }
      else
        {
          return_data_stream_buffer(owning_stream, buffer);
        }
    }

  if(buffer->useCount < 0)
    {
      abort_data_stream(owning_stream);
    }

  if(subscriber_status != ROADRASH_DS_NO_ERROR ||
     result != ROADRASH_DS_NO_ERROR)
    {
      abort_data_stream(owning_stream);
    }

  return ROADRASH_DS_NO_ERROR;
}
