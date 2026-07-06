#include "audio.h"

#include "saudio_subscriber_runtime.h"

long
SetChannelPan(SAudioContext *context,
              long           channel_number,
              long           new_pan)
{
  SAudioOutput *output;
  long left_amplitude;
  long right_amplitude;

  if(new_pan < 0 || new_pan > SAUDIO_PAN_MAX)
    {
      return SAUDIO_ERROR_PAN_OUT_OF_RANGE;
    }

  output = &context->channels[channel_number].output;

  if(output->instrument > 0 && output->muted == 0)
    {
      if(output->channel_count != SAUDIO_MONO_CHANNEL_COUNT)
        {
          return SAUDIO_ERROR_AUDIO_CHANNEL_COUNT;
        }

      if(new_pan < SAUDIO_PAN_MIDPOINT)
        {
          left_amplitude = output->current_amplitude;
          right_amplitude =
            (output->current_amplitude * new_pan) >>
            SAUDIO_PAN_FIXED_SHIFT;
        }
      else
        {
          right_amplitude = output->current_amplitude;
          left_amplitude =
            ((SAUDIO_PAN_FULL_SCALE - new_pan) * right_amplitude) >>
            SAUDIO_PAN_FIXED_SHIFT;
        }

      TweakRawKnob(output->left_envelope_target_knob, left_amplitude);
      TweakRawKnob(output->right_envelope_target_knob,
                   right_amplitude);
    }

  output->current_pan = new_pan;
  return 0;
}
