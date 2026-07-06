#include "saudio_subscriber_runtime.h"

int
get_saudio_channel_status(SAudioContext           *context,
                          SAudioSubscriberMessage *message)
{
  int channel_number;

  channel_number = message->payload.channel.number;
  if(channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
    {
      return (int)context->channels[channel_number].status;
    }
  return 0;
}
