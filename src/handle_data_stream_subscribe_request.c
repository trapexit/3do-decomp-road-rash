#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

int32
handle_data_stream_subscribe_request(DSStreamCBPtr   stream,
                                     DSRequestMsgPtr request)
{
  DSSubscriberPtr subscriber;
  SubscriberMsgPtr message;
  int32 result;
  long subscriber_count;

  result = find_data_stream_subscriber(
    stream, request->msg.subscribe.dataType,
    &subscriber);
  if(result != ROADRASH_DS_NO_SUBSCRIBER_ERROR)
    {
      if(request->msg.subscribe.subscriberPort != 0)
        {
          return ROADRASH_DS_DUPLICATE_SUBSCRIBER_ERROR;
        }
      remove_data_stream_subscriber(stream, subscriber);
      return ROADRASH_DS_NO_ERROR;
    }

  subscriber_count = stream->numSubscribers;
  if(subscriber_count >= DS_MAX_SUBSCRIBERS)
    {
      return ROADRASH_DS_TOO_MANY_SUBSCRIBERS_ERROR;
    }

  subscriber = &stream->subscriber[subscriber_count];
  stream->numSubscribers = subscriber_count + 1;
  subscriber->dataType = request->msg.subscribe.dataType;
  subscriber->subscriberPort =
    request->msg.subscribe.subscriberPort;

  message = (SubscriberMsgPtr)AllocPoolMem(stream->subsMsgPool);
  if(message == 0)
    {
      remove_data_stream_subscriber(stream, subscriber);
      return ROADRASH_DS_NO_MESSAGE_ERROR;
    }

  message->whatToDo = kStreamOpOpening;
  message->privatePtr = request;
  result = SendMsg(
    subscriber->subscriberPort, message->msgItem,
    message, sizeof(SubscriberMsg));
  stream->repliesPending = (result < 0) ? 0 : 1;
  if(result < 0)
    {
      ReturnPoolMem(stream->subsMsgPool, message);
      remove_data_stream_subscriber(stream, subscriber);
    }
  return result;
}
