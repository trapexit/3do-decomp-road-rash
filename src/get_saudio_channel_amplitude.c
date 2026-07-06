#include "saudio_subscriber_runtime.h"

long
GetChannelAmplitude(SAudioContext *context,
                    long           channel_number,
                    long          *amplitude)
{
  SAudioOutput *output;

  output = &context->channels[channel_number].output;
  if(output->muted == 0)
    {
      *amplitude = output->current_amplitude;
    }
  else
    {
      *amplitude = output->saved_amplitude;
    }
  return 0;
}
