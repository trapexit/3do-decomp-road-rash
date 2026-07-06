#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

int32
handle_data_stream_preroll_request(DSStreamCBPtr stream)
{
  DSDataBufPtr buffer;
  DataAcqMsgPtr message;
  int32 result;

  result = ROADRASH_DS_NO_ERROR;
  buffer = take_free_data_stream_buffer(stream);
  while(buffer != 0)
    {
      message = (DataAcqMsgPtr)AllocPoolMem(stream->dataMsgPool);
      if(message == 0)
        {
          return_data_stream_buffer(stream, buffer);
          return ROADRASH_DS_NO_MESSAGE_ERROR;
        }

      message->whatToDo = kAcqOpGetData;
      message->msg.data.bufferPtr = buffer;
      message->privatePtr = 0;
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

      buffer = take_free_data_stream_buffer(stream);
    }
  return result;
}
