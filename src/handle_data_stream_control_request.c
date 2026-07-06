#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

int32
handle_data_stream_control_request(DSStreamCBPtr   stream,
                                   DSRequestMsgPtr request)
{
  DSSubscriberPtr subscriber;
  SubscriberMsgPtr message;
  int32 result;

  result = find_data_stream_subscriber(
    stream, request->msg.control.streamType,
    &subscriber);
  if(result != ROADRASH_DS_NO_ERROR)
    {
      return result;
    }

  message = (SubscriberMsgPtr)AllocPoolMem(stream->subsMsgPool);
  if(message == 0)
    {
      return ROADRASH_DS_NO_MESSAGE_ERROR;
    }

  message->whatToDo = kStreamOpControl;
  message->privatePtr = request;
  message->msg.control.controlArg1 =
    request->msg.control.userDefinedOpcode;
  message->msg.control.controlArg2 =
    request->msg.control.userDefinedArgPtr;

  result = SendMsg(
    subscriber->subscriberPort, message->msgItem,
    message, sizeof(SubscriberMsg));
  stream->repliesPending = (result < 0) ? 0 : 1;
  if(result < 0)
    {
      ReturnPoolMem(stream->subsMsgPool, message);
    }
  return result;
}
