#include "saudio_subscriber_runtime.h"

int
start_saudio_stream_channels(SAudioContext           *context,
                             SAudioSubscriberMessage *message);

int
synchronize_saudio_stream_channels(SAudioContext           *context,
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
      status = FlushChannel(context, channel_number);
      if(status < 0 && result >= 0)
        {
          result = status;
        }
    }
  if(result < 0)
    {
      return result;
    }
  return start_saudio_stream_channels(context, message);
}
