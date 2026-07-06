#include "saudio_subscriber_runtime.h"

int
close_saudio_stream_channels(SAudioContext *context)
{
  int channel_number;
  int result;
  int status;

  result = 0;

  for(channel_number = 0;
      channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT;
      channel_number++)
    {
      status = CloseChannel(context, channel_number);
      if(status < 0 && result >= 0)
        {
          result = status;
        }
    }

  return result;
}
