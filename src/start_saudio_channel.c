#include "saudio_subscriber_runtime.h"
/* StartChannel - reconstructed from the original function at 0x0004B7B0 */

int
StartChannel(SAudioContext *context,
             int            channel_number)
{
  SAudioChannel *channel;
  unsigned int status;

  channel = &context->channels[channel_number];
  status = channel->status;

  if((status & SAUDIO_CHANNEL_ENABLED_FLAG) == 0)
    {
      return (int)context;
    }
  if((status & SAUDIO_CHANNEL_ACTIVE_FLAG) != 0)
    {
      return (int)context;
    }

  channel->status = status | SAUDIO_CHANNEL_ACTIVE_FLAG;
  MoveWaitingMsgsToBufferQueue(context, channel_number);
  return begin_saudio_playback_if_ready(context, channel_number);
}
