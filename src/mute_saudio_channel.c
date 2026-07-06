/* Reconstructed from the original function at 0x0004BCF4. */

#include "saudio_subscriber_runtime.h"

long
MuteChannel(SAudioContext *context,
            long           channel_number,
            boolean        external)
{
  SAudioOutput *output;
  boolean requested_externally;
  long result;

  result = 0;
  output = &context->channels[channel_number].output;
  requested_externally = external;

  if(requested_externally != 0)
    {
      output->external_mute = requested_externally;
    }

  if(output->muted == 0)
    {
      GetChannelAmplitude(context, channel_number,
                          &output->saved_amplitude);
      result = SetChannelAmplitude(context, channel_number, 0);
      output->muted = 1;
    }

  return result;
}
