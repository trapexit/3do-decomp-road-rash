#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

int32
handle_data_stream_connect_request(DSStreamCBPtr   stream,
                                   DSRequestMsgPtr request)
{
  DataAcqMsgPtr message;
  DSDataBufPtr buffer;
  int32 result;

  if(stream->acquirePort == request->msg.connect.acquirePort)
    {
      return ROADRASH_DS_NO_ERROR;
    }

  if(stream->acquirePort != 0)
    {
      message = (DataAcqMsgPtr)AllocPoolMem(stream->dataMsgPool);
      if(message == 0)
        {
          return ROADRASH_DS_NO_MESSAGE_ERROR;
        }

      message->whatToDo = kAcqOpDisconnect;
      message->privatePtr = 0;
      result = SendMsg(
        stream->acquirePort, message->msgItem,
        message, sizeof(DataAcqMsg));
      if(result < 0)
        {
          ReturnPoolMem(stream->dataMsgPool, message);
          return result;
        }
    }

  buffer = take_filled_data_stream_buffer(stream);
  while(buffer != 0)
    {
      return_data_stream_buffer(stream, buffer);
      buffer = take_filled_data_stream_buffer(stream);
    }

  stream->acquirePort = request->msg.connect.acquirePort;
  if(stream->acquirePort == 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }

  message = (DataAcqMsgPtr)AllocPoolMem(stream->dataMsgPool);
  if(message == 0)
    {
      stream->acquirePort = 0;
      return ROADRASH_DS_NO_MESSAGE_ERROR;
    }

  message->whatToDo = kAcqOpConnect;
  message->msg.connect.streamCBPtr = stream;
  message->privatePtr = request;
  result = SendMsg(
    stream->acquirePort, message->msgItem,
    message, sizeof(DataAcqMsg));
  stream->repliesPending = (result < 0) ? 0 : 1;
  if(result < 0)
    {
      ReturnPoolMem(stream->dataMsgPool, message);
      stream->acquirePort = 0;
    }

  return result;
}
