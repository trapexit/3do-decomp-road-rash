#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"

void
remove_data_stream_subscriber(DSStreamCBPtr   stream,
                              DSSubscriberPtr subscriber)
{
  DSSubscriberPtr subscribers;
  int32 subscriber_index;
  int32 remaining_count;
  long subscriber_count;

  subscribers = stream->subscriber;
  subscriber_index = subscriber - subscribers;
  subscriber_count = stream->numSubscribers;
  remaining_count =
    subscriber_count - subscriber_index - 1;
  while(remaining_count > 0)
    {
      *subscriber = subscriber[1];
      subscriber++;
      remaining_count--;
    }

  stream->numSubscribers = subscriber_count - 1;
}
