#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"

int32
abort_data_stream(DSStreamCBPtr stream)
{
  SubscriberMsg message;
  int32 saved_reply_count;
  int32 result;

  stream->streamFlags &= ~ROADRASH_STREAM_ACTIVE_STATE_FLAGS;
  stream->streamFlags |= ROADRASH_STREAM_ABORTED_FLAG;

  message.whatToDo = kStreamOpAbort;
  message.privatePtr = 0;
  saved_reply_count = stream->repliesPending;
  result = broadcast_data_stream_subscriber_message(
    stream, &message);
  stream->repliesPending = saved_reply_count;

  return result;
}
