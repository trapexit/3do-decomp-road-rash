#include "saudio_subscriber_runtime.h"

long
GetChannelPan(SAudioContext *context,
              long           channel_number,
              long          *pan)
{
  *pan = context->channels[channel_number].output.current_pan;
  return 0;
}
