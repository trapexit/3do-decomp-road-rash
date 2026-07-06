#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

int32
handle_data_stream_close_request(DSStreamCBPtr   stream,
                                 DSRequestMsgPtr request)
{
  SubscriberMsg subscriber_message;
  DataAcqMsgPtr acquisition_message;
  int32 result;
  int32 subscriber_result;
  DSRequestMsgPtr queued_request;
  DSRequestMsgPtr next_request;

  stream->streamFlags &= ~ROADRASH_STREAM_ACTIVE_STATE_FLAGS;
  queued_request = (DSRequestMsgPtr)request->link;
  request->link = 0;
  stream->requestMsgTail = request;
  while(queued_request != 0)
    {
      next_request = (DSRequestMsgPtr)queued_request->link;
      ReplyMsg(queued_request->msgItem, ROADRASH_DS_ABORT_ERROR,
               queued_request, sizeof(DSRequestMsg));
      queued_request = next_request;
    }

  result = ROADRASH_DS_NO_ERROR;

  if(stream->acquirePort != 0)
    {
      acquisition_message = (DataAcqMsgPtr)AllocPoolMem(
        stream->dataMsgPool);
      if(acquisition_message != 0)
        {
          acquisition_message->whatToDo = kAcqOpDisconnect;
          acquisition_message->privatePtr = request;
          result = SendMsg(
            stream->acquirePort, acquisition_message->msgItem,
            acquisition_message, sizeof(DataAcqMsg));
          if(result < 0)
            {
              ReturnPoolMem(stream->dataMsgPool, acquisition_message);
            }
          else
            {
              stream->repliesPending++;
              stream->acquirePort = 0;
            }
        }
      else
        {
          result = ROADRASH_DS_NO_MESSAGE_ERROR;
        }
    }

  subscriber_message.whatToDo = kStreamOpClosing;
  subscriber_message.privatePtr = request;
  subscriber_result = broadcast_data_stream_subscriber_message(
    stream, &subscriber_message);
  if(result >= 0)
    {
      result = subscriber_result;
    }
  return result;
}
