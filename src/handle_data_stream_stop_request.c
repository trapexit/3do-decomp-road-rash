#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"

int32
handle_data_stream_stop_request(DSStreamCBPtr   stream,
                                DSRequestMsgPtr request)
{
  SubscriberMsg message;

  if((stream->streamFlags & STRM_CLOCK_WAS_VALID) != 0)
    {
      DSGetClock(stream, &stream->lastValidClock);
    }

  stream->streamFlags &= ~ROADRASH_STREAM_ACTIVE_STATE_FLAGS;

  message.whatToDo = kStreamOpStop;
  message.msgItem = 0;
  message.privatePtr = request;
  message.link = 0;
  message.msg.stop.options = request->msg.stop.options;
  message.msg.control.controlArg2 = 0;

  return broadcast_data_stream_subscriber_message(stream, &message);
}
