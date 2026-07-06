#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

#include "rw_semantic_data.h"

int32
handle_data_stream_marker_request(DSStreamCBPtr   stream,
                                  DSRequestMsgPtr request)
{
  DataAcqMsgPtr message;
  DSDataBufPtr buffer;
  int32 result;

  message = (DataAcqMsgPtr)AllocPoolMem(stream->dataMsgPool);
  if(message == 0)
    {
      return ROADRASH_DS_NO_MESSAGE_ERROR;
    }

  stream->streamFlags &= ~STRM_EOF;

  buffer = take_filled_data_stream_buffer(stream);
  while(buffer != 0)
    {
      return_data_stream_buffer(stream, buffer);
      buffer = take_filled_data_stream_buffer(stream);
    }

  message->privatePtr = request;
  message->whatToDo = kAcqOpGoMarker;
  message->msg.marker.value = request->msg.goMarker.markerValue;
  message->msg.marker.options = request->msg.goMarker.options;

  result = SendMsg(
    stream->acquirePort, message->msgItem,
    message, sizeof(DataAcqMsg));

  if(request->msg.goMarker.markerValue == 0)
    {
      stream_end_of_file_seen = 1;
    }

  stream->repliesPending = (result < 0) ? 0 : 1;
  if(result < 0)
    {
      ReturnPoolMem(stream->dataMsgPool, message);
    }
  return result;
}
