#include "data_stream_server_runtime.h"
#include "msgport.h"
#include "mempool.h"

void
handle_data_stream_subscriber_reply(DSStreamCBPtr    stream,
                                    SubscriberMsgPtr reply,
                                    Message         *message,
                                    int32           *request_result)
{
  DSRequestMsgPtr request;
  DSDataBufPtr buffer;
  DSSubscriberPtr subscriber;

  request = (DSRequestMsgPtr)reply->privatePtr;
  if(request != 0 && request == stream->requestMsgHead)
    {
      if((int32)message->msg_Result < 0 &&
         *request_result >= 0)
        {
          *request_result = message->msg_Result;
        }
      if(reply->whatToDo == kStreamOpOpening &&
         (int32)message->msg_Result < 0 &&
         find_data_stream_subscriber(
           stream, request->msg.subscribe.dataType, &subscriber) == 0)
        {
          remove_data_stream_subscriber(stream, subscriber);
        }
      stream->repliesPending--;
      if(stream->repliesPending == 0 &&
         request->whatToDo != kDSOpCloseStream)
        {
          request = take_next_data_stream_request(stream);
          if(reply->whatToDo == kStreamOpGetChan)
            {
              *request->msg.getChannel.channelStatusPtr =
                message->msg_Result;
            }
          ReplyMsg(request->msgItem, *request_result, request,
                   sizeof(DSRequestMsg));
        }
    }
  else if(reply->whatToDo == kStreamOpData)
    {
      buffer = (DSDataBufPtr)reply->privatePtr;
      release_subscriber_data_stream_buffer(
        stream, message->msg_Result, buffer);
    }

  ReturnPoolMem(stream->subsMsgPool, reply);
}
