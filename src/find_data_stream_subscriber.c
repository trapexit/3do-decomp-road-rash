#include "data_stream_runtime.h"
#include "data_stream_server_runtime.h"

int32
find_data_stream_subscriber(DSStreamCBPtr    stream,
                            DSDataType       stream_type,
                            DSSubscriberPtr *subscriber)
{
  int32 subscriber_index;
  int32 subscriber_count;
  DSSubscriberPtr candidate;

  subscriber_count = stream->numSubscribers;
  subscriber_index = 0;
  while(subscriber_index < subscriber_count)
    {
      candidate = &stream->subscriber[subscriber_index];
      if(candidate->dataType == stream_type)
        {
          *subscriber = candidate;
          return ROADRASH_DS_NO_ERROR;
        }
      subscriber_index++;
    }

  *subscriber = 0;
  return ROADRASH_DS_NO_SUBSCRIBER_ERROR;
}
