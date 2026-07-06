#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

int32
request_data_stream_buffer_fill(DSStreamCBPtr stream,
                                DSDataBufPtr  buffer)
{
  uint32 flags;
  DataAcqMsgPtr message;
  int32 result;

  if(stream->acquirePort != 0)
    {
      flags = stream->streamFlags;
      if((flags & STRM_RUNNING) != 0 &&
         (flags & STRM_EOF) == 0)
        {
          message = (DataAcqMsgPtr)AllocPoolMem(
            stream->dataMsgPool);
          if(message == 0)
            {
              return_data_stream_buffer(stream, buffer);
              return ROADRASH_DS_NO_MESSAGE_ERROR;
            }

          message->whatToDo = kAcqOpGetData;
          message->privatePtr = 0;
          message->msg.data.bufferPtr = buffer;
          message->msg.data.bufferSize = stream->bufDataSize;
          result = SendMsg(
            stream->acquirePort, message->msgItem,
            message, sizeof(DataAcqMsg));
          if(result < 0)
            {
              ReturnPoolMem(stream->dataMsgPool, message);
              return_data_stream_buffer(stream, buffer);
              return result;
            }
          return ROADRASH_DS_NO_ERROR;
        }
    }

  return_data_stream_buffer(stream, buffer);
  return ROADRASH_DS_NO_ERROR;
}
