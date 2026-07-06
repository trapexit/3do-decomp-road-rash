#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

int32
broadcast_data_stream_subscriber_message(DSStreamCBPtr    stream,
                                         SubscriberMsgPtr template_message)
{
  int32 subscriber_index;
  int32 result;
  SubscriberMsgPtr message;
  DSSubscriberPtr subscriber;

  if(stream->subsMsgPool->numFreeInPool <
     stream->numSubscribers)
    {
      return ROADRASH_DS_NO_MESSAGE_ERROR;
    }

  subscriber_index = 0;
  if(stream->numSubscribers <= 0)
    {
      return ROADRASH_DS_NO_ERROR;
    }

  do
    {
      subscriber = &stream->subscriber[subscriber_index];
      message = (SubscriberMsgPtr)AllocPoolMem(
        stream->subsMsgPool);
      if(message == 0)
        {
          return ROADRASH_DS_NO_MESSAGE_ERROR;
        }

      template_message->msgItem = message->msgItem;
      *message = *template_message;

      result = SendMsg(
        subscriber->subscriberPort, message->msgItem,
        message, sizeof(SubscriberMsg));
      if(result < 0)
        {
          ReturnPoolMem(stream->subsMsgPool, message);
          return result;
        }

      stream->repliesPending++;
      subscriber_index++;
    } while(subscriber_index < stream->numSubscribers);

  return result;
}
