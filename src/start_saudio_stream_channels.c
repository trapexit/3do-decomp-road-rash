#include "saudio_subscriber_runtime.h"

int
start_saudio_stream_channels(SAudioContext           *context,
                             SAudioSubscriberMessage *message)
{
  int channel_number;
  int result;
  int status;

  result = 0;

  for(channel_number = 0;
      channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT;
      channel_number++)
    {
      if((message->payload.start.options & SAUDIO_STREAM_FLUSH_OPTION) !=
         0)
        {
          status = FlushChannel(context, channel_number);
          if(status < 0)
            {
              if(result >= 0)
                {
                  result = status;
                }
              continue;
            }
        }
      status = StartChannel(context, channel_number);
      if(status < 0 && result >= 0)
        {
          result = status;
        }
    }

  return result;
}
