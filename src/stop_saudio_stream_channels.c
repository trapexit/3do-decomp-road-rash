#include "saudio_subscriber_runtime.h"

int
stop_saudio_stream_channels(SAudioContext           *context,
                            SAudioSubscriberMessage *message)
{
  int channel_number;
  int result;
  int status;

  result = 0;

  OrphanPlayingBuffers(context);
  for(channel_number = 0;
      channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT;
      channel_number++)
    {
      if((message->payload.stop.options & SAUDIO_STREAM_FLUSH_OPTION) !=
         0)
        {
          status = FlushChannel(context, channel_number);
        }
      else
        {
          status = StopChannel(context, channel_number);
        }
      if(status < 0 && result >= 0)
        {
          result = status;
        }
    }

  return result;
}
