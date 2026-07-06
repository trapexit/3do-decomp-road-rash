#include "saudio_subscriber_runtime.h"

long
UnMuteChannel(SAudioContext *context,
              long           channel_number,
              boolean        external)
{
  SAudioOutput *output;
  boolean requested_externally;
  long result;

  output = &context->channels[channel_number].output;
  requested_externally = external;

  if(output->muted != 0)
    {
      if(((requested_externally != 0) &&
          (output->external_mute != 0)) ||
         ((requested_externally == 0) &&
          (output->external_mute == 0)))
        {
          output->muted = 0;
          result = SetChannelAmplitude(context, channel_number,
                                       output->saved_amplitude);
          output->external_mute = 0;
          return result;
        }
    }

  return 0;
}
