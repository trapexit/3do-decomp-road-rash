#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

void
handle_data_stream_acquisition_reply(DSStreamCBPtr stream,
                                     DataAcqMsgPtr reply,
                                     Message      *message,
                                     int32        *request_result)
{
  DSRequestMsgPtr request;
  DSDataBufPtr buffer;
  Err status;
  uint32 flags;

  request = (DSRequestMsgPtr)reply->privatePtr;
  if(request != 0 && request == stream->requestMsgHead)
    {
      if((int32)message->msg_Result < 0 &&
         *request_result >= 0)
        {
          *request_result = message->msg_Result;
        }
      if(reply->whatToDo == kAcqOpConnect && (int32)message->msg_Result < 0)
        {
          stream->acquirePort = 0;
        }
      stream->repliesPending--;
      if(stream->repliesPending == 0 &&
         request->whatToDo != kDSOpCloseStream)
        {
          request = take_next_data_stream_request(stream);
          ReplyMsg(
            request->msgItem, *request_result, request,
            sizeof(DSRequestMsg));
        }
    }
  else if(reply->whatToDo == kAcqOpGetData)
    {
      buffer = reply->msg.data.bufferPtr;
      status = message->msg_Result;
      if(status == ROADRASH_DS_NO_ERROR)
        {
          buffer->next = 0;
          if(stream->filledBufHead != 0)
            {
              stream->filledBufTail->next = buffer;
            }
          else
            {
              stream->filledBufHead = buffer;
            }
          stream->filledBufTail = buffer;
        }
      else
        {
          return_data_stream_buffer(stream, buffer);
          status = message->msg_Result;
          if(status == ROADRASH_DS_ABORT_ERROR)
            {
              flags = stream->streamFlags;
              flags &= ~ROADRASH_STREAM_ACTIVE_STATE_FLAGS;
              flags |= STRM_EOF | ROADRASH_STREAM_ABORTED_FLAG;
              stream->streamFlags = flags;
            }
          else if(status == ROADRASH_DS_END_OF_FILE_ERROR)
            {
              stream->streamFlags |= STRM_EOF;
            }
          else if(status != ROADRASH_DS_WAS_FLUSHED_ERROR)
            {
              flags = stream->streamFlags;
              flags &= ~ROADRASH_STREAM_ACTIVE_STATE_FLAGS;
              flags |= STRM_EOF;
              stream->streamFlags = flags;
            }
        }
    }

  ReturnPoolMem(stream->dataMsgPool, reply);
}
