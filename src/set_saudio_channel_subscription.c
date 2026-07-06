#include "saudio_subscriber_runtime.h"

int
set_saudio_channel_subscription(SAudioContext           *context,
                                SAudioSubscriberMessage *message)
{
  SAudioChannel *channel;
  unsigned int previous_status;
  int channel_number;

  channel_number = message->payload.channel.number;
  if(!SAUDIO_CHANNEL_NUMBER_IS_VALID(channel_number))
    {
      return SAUDIO_ERROR_CHANNEL_OUT_OF_RANGE;
    }
  channel = &context->channels[channel_number];

  if(channel_number < SAUDIO_SUBSCRIBER_CHANNEL_COUNT)
    {
      previous_status = channel->status;
      channel->status |=
        (unsigned int)message->payload.channel.status &
        ~SAUDIO_CHANNEL_SYSTEM_BITS;

      if((previous_status & SAUDIO_CHANNEL_ENABLED_FLAG) != 0)
        {
          if((channel->status & SAUDIO_CHANNEL_ENABLED_FLAG) == 0)
            {
              FlushChannel(context, channel_number);
            }
        }
      else if((channel->status & SAUDIO_CHANNEL_ENABLED_FLAG) != 0)
        {
          StartChannel(context, channel_number);
        }
    }

  return 0;
}
