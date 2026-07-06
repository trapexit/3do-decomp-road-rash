#include "audio.h"
/* Reconstructed from the original function at 0x0004BB4C. */

#include "saudio_subscriber_runtime.h"

long
SetChannelAmplitude(SAudioContext *context,
                    long           channel_number,
                    long           new_amplitude)
{
  SAudioOutput *output;
  long left_amplitude;
  long right_amplitude;

  if(new_amplitude < 0 || new_amplitude > SAUDIO_AMPLITUDE_MAX)
    {
      return SAUDIO_ERROR_AMPLITUDE_OUT_OF_RANGE;
    }

  output = &context->channels[channel_number].output;

  if(output->instrument > 0 && output->muted == 0)
    {
      if(output->channel_count == SAUDIO_STEREO_CHANNEL_COUNT)
        {
          TweakRawKnob(output->left_envelope_target_knob,
                       new_amplitude);
        }
      else
        {
          if(output->current_pan < SAUDIO_PAN_MIDPOINT)
            {
              left_amplitude = new_amplitude;
              right_amplitude =
                (output->current_pan * new_amplitude) >>
                SAUDIO_PAN_FIXED_SHIFT;
            }
          else
            {
              right_amplitude = new_amplitude;
              left_amplitude =
                ((SAUDIO_PAN_FULL_SCALE - output->current_pan) *
                 new_amplitude) >> SAUDIO_PAN_FIXED_SHIFT;
            }
          TweakRawKnob(output->left_envelope_target_knob,
                       left_amplitude);
          TweakRawKnob(output->right_envelope_target_knob,
                       right_amplitude);
        }
    }

  output->current_amplitude = new_amplitude;
  if(output->muted != 0)
    {
      output->saved_amplitude = new_amplitude;
    }

  return 0;
}
