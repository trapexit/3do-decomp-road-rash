#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"

int32
handle_data_stream_start_request(DSStreamCBPtr   stream,
                                 DSRequestMsgPtr request)
{
  uint32 flags;
  SubscriberMsg message;

  flags = stream->streamFlags;
  if((flags & STRM_RUNNING) != 0)
    {
      return ROADRASH_DS_WAS_RUNNING_ERROR;
    }

  if((flags & ROADRASH_STREAM_ABORTED_FLAG) != 0)
    {
      abort_data_stream(stream);
      return ROADRASH_DS_ABORT_ERROR;
    }

  flags |= STRM_RUNNING;
  stream->streamFlags = flags;
  flags &= ~STRM_CLOCK_VALID;
  stream->streamFlags = flags;

  if((flags & STRM_CLOCK_WAS_VALID) != 0)
    {
      DSSetClock(stream, stream->lastValidClock);
    }
  if((request->msg.start.options &
      ROADRASH_DS_START_RESET_CLOCK_OPTION) != 0)
    {
      DSSetClock(stream, 0);
    }

  message.whatToDo = kStreamOpStart;
  message.privatePtr = request;
  message.msg.start.options = request->msg.start.options;
  return broadcast_data_stream_subscriber_message(stream, &message);
}
